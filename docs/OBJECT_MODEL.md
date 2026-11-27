# The Coda-C object model

How Coda-C objects, classes and virtual calls work underneath. You do not need this to use the library, but it explains the rules in [USAGE.md](USAGE.md) and helps when you debug or contribute.

This page describes the implementation of Coda-C 3.6 on a 64-bit system. Treat the layout as an implementation detail that can change between versions. Code that only uses the functions in [API.md](API.md) is not affected.

## Objects and their header

Every object is one block of memory with a 16-byte header in front of the storage. The pointer you hold, the `Obj`, points just past the header.

```text
allocation start
|
v
+------------------+---------------------------+
| header (16 bytes)| storage (size bytes)      |
+------------------+---------------------------+
                   ^
                   the Obj / Char / Array pointer you hold
```

The header holds:

| Field | Size | Meaning |
| --- | --- | --- |
| `clas` | 8 bytes | Pointer to the object's class record. |
| `size` | 24 bits | Size in bytes of this object's storage. |
| `fsegs` | 8 bits | How many superclass parts follow this one. 0 for a plain object. |
| `count` | 16 bits, atomic | The retain count. |
| `metasize` | 16 bits | Distance from the object pointer back to the start of the allocation. |

Memory is allocated with `calloc`, so new objects start zeroed. `Object_Counter` goes up by one for each allocation and down by one when an object is destroyed. When `Memory_SecurityOn` is true (the default), the whole block is overwritten with zeros before it is freed.

The 16-bit retain count and the 24-bit size are the source of the limits in [LIMITS.md](LIMITS.md).

## String constants

`Os("text")` builds the string `"\0\0text"` and yields a pointer to the `t`. The two zero bytes in front are where an object's `metasize` field would be. Every function that takes an object looks at those two bytes first. If both are zero, the value is a constant: its class is `ConstChar`, it has no header and no retain count, and keeping or freeing it does nothing.

This is also why a plain C string must never be passed where an object is expected. The library would read two bytes in front of it that belong to something else.

## Classes and subclasses

A class is described by a record, the `CodaCLASS` struct in `coda-c.h`:

| Member | Meaning |
| --- | --- |
| `dtor` | Destroys this class's own fields. |
| `kClass` | The class name, returned by `kindO`. |
| `superClass` | The superclass record, or `NULL` for a root class. |
| `etor`, `ekeep` | For containers: release and retain one element. |
| `size` | `sizeof` the class's storage. |
| `superNel` | How many units of the superclass's storage this class uses. 1 for every class made with `CodaClass`. |
| `bits` | Special-class marker, see below. |
| `itor` | Initializes a new object. |
| `kize` | For variable-size classes: the fixed part of the storage. |
| `classObject` | The class's own object. Holds its table of virtual functions. |
| `properties` | An optional dictionary for class properties. `CodaClass` sets it to 0. |

### Class bits

| `bits` value | Name | Meaning |
| --- | --- | --- |
| -1 | `bits_Root` | A root class. The base of an inheritance chain. |
| -2 | `bits_Trans` | A transparent subclass. It adds functions but no storage. `ConstChar` is one. |
| 3 | `bits_Zerok` | Objects of this class may be created with a count of 0. |
| 4 | `bits_Etor2` | The `etor` and `ekeep` hooks also receive the container. |

### Variable-size classes (`kize`)

For most classes `newOC(class,n)` makes an array of `n` storage units. For a class with a `kize`, the storage is the fixed `kize` bytes plus `n` extra bytes. `Char` and `Keyword` work this way: a `Keyword` is a small struct followed by the key text.

### Subclass objects

A subclass object is a single allocation holding one part for each class in its chain, and each part has its own header. The most derived part comes first in memory, followed by the parts of its superclasses.

```text
+--------+-----------+--------+-----------+
| header | Savings   | header | Account   |
| (Sav.) | storage   | (Acct.)| storage   |
+--------+-----------+--------+-----------+
         ^                    ^
         Savings pointer      Account part (superO of the Savings pointer)
```

- The pointer from `newO(Savings)` points at the most derived part. This is the object's *normal address*.
- `superO(obj)` steps to the next part. `spindleO(obj,&Isa_Account)` finds the part for a given class. `normalO(obj)` goes back to the normal address.
- A virtual function implemented by `Account` receives the `Account` part as its `self`, even when you called it on a `Savings`. That is why code in a base class that will be subclassed must reach its fields through `self$`, which the `$` property macros do.
- Each header is 16 bytes, so the storage of every part except the last (the root class's) must be a multiple of 8 bytes for the header after it to stay aligned. `SubClassOk()` checks this at compile time for the subclass it follows. A base class can have any size, since nothing follows it, which is why the `xyzperson.c` example can use a one-byte base.

### Containers

A container class gives the library two hooks, `ekeep` to retain an element and `etor` to release one. `Array` uses `keepO` and `freeO`. `CArray` and `CDictionary` define neither, which is why they never retain or free their contents. A class with `bits_Etor2` has hooks that also receive the container, for elements whose cleanup depends on it.

## Virtual calls

Each class owns a small hash table, in its class object, mapping signatures to functions. A signature is identified by the **address** of its key constant (`sig_name`), not by the characters in it.

- `method$` and `CodaSig` insert `signature -> function` into the class's table, at program start.
- `obj_(name,obj,...)` looks `sig_name` up in the object's class, then in its superclass, and so on up the chain. The first match is called with the matching part of the object as `self`.
- `super_(name,obj,...)` starts the same search one class above the current one.
- If nothing matches, `obj_` aborts with `does not respond to`. `OResponds` does the same search and returns the function or `NULL` instead.

Because the key is compared by address, each signature must have exactly one definition (`typedef ... OSig$(name,...)`, which creates the global `sig_name`) and `extern ConstChar sig_name;` declarations everywhere else. The [`coda-h`](CODA-H.md) tool maintains those declarations.

## Class registration

`CodaClass(...)` ends with a function marked as a constructor, so every class registers itself in `Class_dictionary()` before `main` runs. Signatures register in `Signature_dictionary()` the same way. The `examples/codainfo.c` program prints both.

## Array internals

An `Array` stores its elements in one block of pointers with spare room at the front and the back. Inserting or removing at either end moves nothing. The block grows by a factor of 1.6 when full and shrinks when it is a tenth full or less. The storage is ordinary `calloc` memory, not an object, which is why an array can hold more than the 8 MB single-object limit.

## Dictionary internals

A `Dictionary` keeps its entries in a linked list, newest first, so scanning always works. When it reaches 32 keys it adds an open-addressing hash index over the same entries, using FNV-1a with a final mixing step. Deleted slots are marked and reused, and the index is rebuilt when it fills or becomes much larger than needed. Lookup in the index compares the key text, so collisions only slow things down.
