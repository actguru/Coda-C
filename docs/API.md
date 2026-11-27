# Coda-C API reference

Reference for Coda-C 3.6. Function declarations below are taken directly from `coda-c.h`. For tutorials see [USAGE.md](USAGE.md).

- [Conventions](#conventions)
- [Memory and ownership](#memory-and-ownership)
- [Strings](#strings)
- [Array and CArray](#array-and-carray)
- [Dictionary, CDictionary and Keyword](#dictionary-cdictionary-and-keyword)
- [Pointer](#pointer)
- [Classes, inheritance and virtual calls](#classes-inheritance-and-virtual-calls)
- [Class definition macros](#class-definition-macros)
- [Messages, errors and assertions](#messages-errors-and-assertions)
- [Utilities](#utilities)
- [Registries and diagnostics](#registries-and-diagnostics)
- [Deprecated and internal](#deprecated-and-internal)

## Conventions

- `Obj` is `void *`. A class name such as `Array` or `Char` is already a pointer type, so write `Array list;`, not `Array *list;`.
- Functions are named `Class_function`. Many have a short alias, listed in the tables below.
- **New** means the function returns a new object with a retain count of 1. Release it with `freeO` or declare the variable `cleanO`. A few of these functions return a string constant in unusual cases (for example `ToContainer`), so always release results with `freeO`. It does nothing to a constant.
- **Retains** means the callee keeps its own reference (`keepO`). **Takes** means the callee takes over your reference. Declarations mark taken arguments with `$CONSUMED`.
- Functions that take an index abort on an invalid one unless the entry says otherwise. See [LIMITS.md](LIMITS.md#errors-abort-the-program).
- Functions given a `NULL` object return a neutral value where stated, such as a count of 0.

## Memory and ownership

```c
Obj Memory_alocO(int size);
Obj Memory_newO(Obj obj,int nel);
void Memory_free(Obj obj);
Obj Memory_keep(Obj obj);
void Memory_clean(const void *vp);
int4 Memory_count(Obj obj);
int4 Memory_size(Obj obj);
CodaCLASS* Memory_class(Obj obj);
Obj Memory_superObject(Obj obj);
Obj Memory_normalize(Obj obj);
Obj Memory_spindle(Obj obj,CodaCLASS* newClass);
Obj Memory_objectToClass(Obj obj,CodaCLASS* newClass);
CodaCLASS* Memory_reclass(Obj obj,CodaCLASS *clas);
Obj Memory_aloc(int size,CodaCLASS *clas,int extra);
int Memory_align(int pow2);
$RETNONZERO pointer allocC(size_t size);
void freeC(pointer address);
void clean_C(const void *vp);
```

| Macro | Expands to | Meaning |
| --- | --- | --- |
| `newO(class)` | `Memory_newO(Class_class,1)` | New zeroed object of a class. **New.** |
| `newOC(class,count)` | `Memory_newO(Class_class,count)` | New object with `count` elements, or `count` extra bytes for `kize` classes such as `Char`. **New.** `count` of 0 aborts unless the class is marked `CodaClass_Zerok()`. |
| `alocO(size)` | `Memory_alocO(size)` | New classless zeroed object of `size` bytes (class `Void`). **New.** |
| `keepO(obj)` | `Memory_keep(obj)` | Retain count +1. Returns `obj`. Safe for `NULL` and `Os` constants. |
| `freeO(obj)` | `Memory_free(obj)` | Retain count -1. Destroys the object when it reaches 0. Safe for `NULL` and `Os` constants. |
| `countO(obj)` | `Memory_count(obj)` | Retain count. `-1` for `NULL`, `0` for `Os` constants. |
| `sizeO(obj)` | `Memory_size(obj)` | Storage size in bytes of this object's segment. `0` for `NULL`. For `Os` constants, the string length plus 1. |
| `classO(obj)` | `Memory_class(obj)` | The class record (`CodaCLASS *`). Never `NULL`: classless objects give the `Void` class. |
| `kindO(obj)` | `classO(obj)->kClass` | The class name as a C string. |
| `superO(obj)` | `Memory_superObject(obj)` | The superclass part of a subclass object, or `NULL`. |
| `normalO(obj)` | `Memory_normalize(obj)` | The object's standard address, converting a superclass part back to the whole. |
| `spindleO(obj,&Isa_Class)` | `Memory_spindle(obj,class)` | The part of the object belonging to that class, or `NULL`. |
| `cleanO` | attribute | Declares an `Obj` variable released automatically at the end of its scope. The variable is `const`. |
| `cleanC` | attribute | Same for memory from `allocC`, released with `freeC`. |
| `setO(var,obj)` | | `keepO(obj)`, `freeO(var)`, `var=obj`. Safe when both are the same object. |
| `setO_take(var,obj)` | | `freeO(var)`, `var=obj`. Takes your reference. |
| `isOs(obj)` | | True for a string constant made by `Os()`. |

`Object_Counter` (`_Atomic huge`) is the number of live objects. `Memory_SecurityOn` (default true) scrubs an object's memory when it is destroyed.

Notes:

- `allocC` is `calloc` that aborts when memory runs out. `freeC(NULL)` is allowed.
- `Memory_reclass` changes an object's class record and returns the old one. `Memory_objectToClass` finds the part of an object that belongs to another class. `Memory_align` sets an alignment requirement checked when subclass objects are built. These are advanced tools.

## Strings

```c
Char Char_Value(const char *string);
Char $FORMAT12 Char_F(char *cs,...);
Char Char_NewBlock(Char self,int count,pointer address);
Char Array_Join(Array array,char *delimiter);
```

| Function | Description |
| --- | --- |
| `Char_Value(s)` | **New** string copying `s`. `NULL` in, `NULL` out. |
| `Char_F(format,...)` | **New** string built like `printf`. Output over 4,095 characters is cut off. |
| `Char_NewBlock(proto,count,address)` | **New** `Char` object of `count` bytes copied from `address`. The bytes are copied as given with no terminator added. `NULL` when `count` is 0 or less. |
| `Array_Join(array,delimiter)` | **New** string joining the `Char` elements of an array with `delimiter`. Non-string elements are skipped. `""` if there are none, `NULL` for a `NULL` array. |
| `Os("text")` | A string constant object. Takes a string literal only. Never freed, `countO` is 0, `kindO` is `ConstChar`. |

`ConstChar` is a transparent subclass of `Char`, so every `Char` function accepts an `Os` constant. Do not modify the characters of an `Os` constant.

## Array and CArray

`Array` retains the objects it holds. `CArray` has the same functions but holds raw pointers and never retains, releases or frees them. It is the right class for any pointer that is not a Coda-C object.

```c
int Array_get_count(Array self);
Obj Array_subInt(Array self,int ix);
Obj Array_addObject(Array self,Obj obj);
void Array_takeObject(Array self,Obj $CONSUMED obj);
Obj Array_insertAt(Array self,int index,Obj obj);
Obj Array_replaceAt(Array self,int ix,Obj obj);
void Array_removeAt(Array self,int dix);
bool Array_removeLast(Array self);
int Array_removeObject(Array self,Obj object);
void Array_removeAll(Array self);
void Array_insertBlock(Array self,int index,int count,pointer block);
void Array_takeBlock(Array self,int index,int count,pointer block);
void Array_removeBlock(Array self,int dix,int count);
Array Array_NewBlock(Array proto,int count,pointer block);
pointer Array_rawAddress(Array self);
void Array_set_name(Array self,char* name);
Char Array_get_name(Array self);
void Array_toaSet(Array self);
bool Array_isaSet(Array self);
Char Array_Diag(Array array);
```

| Function (aliases) | Behavior |
| --- | --- |
| `Array_get_count` (`Array_count`) | Number of elements. `0` for a `NULL` array. |
| `Array_subInt` (`Array_sub`) | Element at `ix`, or `NULL` when `ix` is out of range. |
| `Array_addObject` (`Array_add`) | Appends and **retains**. Returns the object. `NULL` elements are accepted. |
| `Array_takeObject` (`Array_take`) | Appends and **takes** your reference. |
| `Array_insertAt` | Inserts at `index` (0 through the count) and **retains**. Aborts on any other index. |
| `Array_replaceAt` | Replaces element `ix`: retains the new object and releases the old. `ix` must be valid. |
| `Array_removeAt` | Removes and releases the element at `dix`. |
| `Array_removeLast` | Removes and releases the last element. Returns `false` on an empty array. |
| `Array_removeObject` | Removes every element equal to `object` (same address). Returns how many. |
| `Array_removeAll` | Removes and releases every element. |
| `Array_insertBlock` / `Array_takeBlock` | Insert `count` pointers from `block` at `index`, retaining them or taking them. |
| `Array_removeBlock` | Removes `count` elements starting at `dix`. Elements are released last to first. |
| `Array_NewBlock` | **New** array from a block of pointers. A `proto` of `NULL` makes an `Array`; pass a `CArray` object for a raw one. |
| `Array_rawAddress` | Pointer to the internal vector. Invalid after the array changes. |
| `Array_set_name` / `Array_get_name` (`Array_name`) | Optional name used in diagnostics. A `NULL` or empty name clears it. The name is copied. |
| `Array_toaSet` / `Array_isaSet` | Mark an array as a set and test the mark. This is a label only. The library does not keep elements unique. |

Removal and destruction run the element release in reverse order, last element first.

## Dictionary, CDictionary and Keyword

`Dictionary` maps string keys to objects. Keys are copied. `CDictionary` is the same with raw pointer values that are never retained or freed.

```c
void Dictionary_setKey(Dictionary self,char *key,Obj obj);
void Dictionary_takeKey(Dictionary self,char *key,Obj $CONSUMED obj);
Obj Dictionary_subKey(Dictionary self,char *key);
bool Dictionary_removeKey(Dictionary self,char *key);
int Dictionary_removeObject(Dictionary self,Obj obj);
int Dictionary_get_count(Dictionary self);
Pointer Dictionary_AllKeys(Dictionary self);
Keyword Dictionary_scan(Dictionary dict);
Keyword Dictionary_next(Keyword element);
Keyword Dictionary_keywordSub(Dictionary dict,int index);
void Dictionary_set_name(Dictionary self,char* name);
Char Dictionary_get_name(Dictionary self);
void Dictionary_auto(Dictionary dict);
int4 Dictionary_autoDisable(Dictionary dict,bool disable);
void Dictionary_hash(Dictionary dict,uint32_t (*hasher)(const char *string));
void Dictionary_hash_default(Dictionary dict);
void Dictionary_hash_build(Dictionary dict);
Char Dictionary_Diag(Dictionary dict);
```

| Function (aliases) | Behavior |
| --- | --- |
| `Dictionary_setKey` (`Dict_set`) | Sets `key` to `obj`, copying the key and **retaining** the object. An existing key is replaced and its old value released. A `NULL` object removes the key. |
| `Dictionary_takeKey` (`Dict_take`) | As `Dict_set` but **takes** your reference. |
| `Dictionary_subKey` (`Dict_sub`) | The value for `key`, or `NULL`. |
| `Dictionary_removeKey` | Removes `key` and releases its value. Returns `true` if it was present. |
| `Dictionary_removeObject` | Removes every key whose value is `obj` (same address). Returns how many. |
| `Dictionary_get_count` (`Dictionary_count`) | Number of keys. `0` for `NULL`. |
| `Dictionary_AllKeys` | **New** `Pointer` of the key strings in the order added, or `NULL` when empty. The strings belong to the dictionary. |
| `Dictionary_scan`, `Dictionary_next` | Walk the entries from newest to oldest as `Keyword` pointers. Do not change the dictionary while walking. |
| `Dictionary_keywordSub(dict,index)` | The `index`th entry in scan order, or `NULL`. Slow for large dictionaries. |
| `Dictionary_set_name` / `Dictionary_get_name` (`Dictionary_name`) | Optional name used in diagnostics. |

`Dictionary_scan` and friends return a raw `Keyword` view with the members `item` (the value) and `word` (the key). Use it only to read:

```c
for(Keyword k=Dictionary_scan(dict);k;k=Dictionary_next(k))
	printf("%s=%p\n",k->word,k->item);
```

**Hashing.** A dictionary starts as a linked list. When it reaches 32 keys it builds a hash index automatically, using FNV-1a with a final mixing step. You can control this:

| Function | Behavior |
| --- | --- |
| `Dictionary_autoDisable(dict,true)` | Stops the automatic switch to hashing. Returns the threshold, 32. |
| `Dictionary_hash_default(dict)` | Turns hashing on with the built-in hash now. |
| `Dictionary_hash(dict,hasher)` | Turns hashing on with your own `uint32_t (*)(const char *)`. A `NULL` hasher drops the index and returns to the plain list; combine it with `Dictionary_autoDisable` to keep it off. |
| `Dictionary_hash_build(dict)` | Rebuilds the index. Called for you when needed. |

A custom hasher must return `uint32_t` and must give equal strings equal values. Keys that hash alike are handled correctly, only slower.

**Keyword objects.** `Keyword_Value(word,item)` and `Keyword_TakeItem(word,item)` create standalone `Keyword` objects:

```c
Keyword Keyword_Value(char *word,Obj item);
Keyword Keyword_TakeItem(char *word,Obj item);
void Keyword_set_item(Keyword self,Obj object);
Obj Keyword_get_item(Keyword self);
```

`Keyword_Value` retains `item`, `Keyword_TakeItem` takes it, and the keyword releases it when destroyed. `Keyword_item` is an alias for `Keyword_get_item`. `Dictionary_keywordFromWord(word)` turns the `word` member of a scanned keyword back into its `Keyword`.

## Pointer

A `Pointer` object is a counted block of `void *` values, useful for passing results between functions.

```c
Pointer Pointer_Value(pointer value);
Pointer Pointer_NewBlock(Pointer self,int count,pointer block);
int Pointer_length(pointer address);
int Root_get_count(Root self);
```

| Function | Behavior |
| --- | --- |
| `Pointer_Value(p)` | **New** `Pointer` holding one value. |
| `Pointer_NewBlock(proto,count,block)` | **New** `Pointer` copying `count` pointers. `proto` is ignored. `NULL` when `count` is 0 or less. |
| `Pointer_count(p)` | Number of pointers (alias of `Root_count`). |
| `Pointer_length(address)` | Length of a `NULL`-terminated array of pointers, counted up to the first `NULL`. `0` for `NULL`. |

A `Pointer` can be indexed like an array: `((char **)keys)[j]`.

## Classes, inheritance and virtual calls

```c
bool OisaClass(Obj obj,Obj classobj);
void *ORespond3(Obj obj,char *sel,Obj *spindle,int noDie,CodaCLASS *superOf);
void codac_sig(pointer cmux,char *key,pointer data);
```

| Macro | Meaning |
| --- | --- |
| `isa_(obj,Class)` | True if the object's class is `Class` or inherits from it. |
| `OSig$(name,args...)` | Declares the signature of a virtual function. Written as `typedef ReturnType OSig$(name,args);`. Defines the key `sig_name`. |
| `OSig(name)` | The function-pointer form `(*name)`, used when writing a signature by hand. |
| `obj_(name,obj,args...)` | Calls the virtual function `name` on `obj`. Aborts if the object does not implement it. |
| `super_(name,obj,args...)` | Calls the superclass's version. Only valid inside a class definition. |
| `OResponds(obj,name)` | The function pointer if `obj` implements `name`, else `NULL`. |
| `OHasSuper(obj,name)` | Same, for the superclass's version. Only valid inside a class definition. |
| `method$(type,name,args)` | Defines and registers a static implementation of `name` for the current class. |
| `method$def(type,name,args)` | Defines a static implementation without registering it. |
| `function$(name,args)` | Declares `Class_name(Class self,args)`, an ordinary non-virtual function. `$(name,args)` is the short form. |
| `CodaSig(name)` | Registers the existing function `Class_name` as the current class's implementation of `name`. Use inside a `$boot` block. |
| `CodaAka(name,Other)` | Registers `Other_name` as the current class's implementation of `name`. |
| `$boot(name)` | A function that runs at program start, before `main`. |
| `$boot1(name)` | A `$boot` that registers one signature. |

A signature is identified by the address of its `sig_name` key, so define it in one source file and declare it `extern ConstChar sig_name;` elsewhere. The [`coda-h`](CODA-H.md) tool writes these declarations.

## Class definition macros

| Macro | Purpose |
| --- | --- |
| `CodaClassDef(Name,Storage,Super)` | Declares the class type and its records. Needed before the definition when a `dtor` uses `self`. |
| `CodaClass(Name,Storage,Super)` | Completes the definition and registers the class at program start. |
| `CodaClassZeros(a,b,...)` | Marks the listed hooks as not defined. Names: `dtor`, `itor`, `kize`, `etor`, `ekeep`, `bits`. |
| `CodaClassZerosC()` | `CodaClassZeros(dtor,itor,kize,etor,ekeep,bits)`: a class with no hooks. |
| `CodaClassTransC()` | Transparent class with no hooks: adds virtual functions without storage. |
| `CodaClass_Trans()` | Sets `bits` to the transparent value. |
| `CodaClass_Root()` | Sets `bits` to the root value. For the library's root classes. |
| `CodaClass_Zerok()` | Allows `newOC(class,0)`. |
| `SubClassOk()` | Compile-time check that the subclass storage is a multiple of the pointer size. Put one after every subclass. |
| `property$_(type,var)` | Getter `Class_var()` and setter `Class_set_var()` for a plain value. |
| `property$$(type,var)` | Same, for a class that may be subclassed. |
| `propertyO_(type,var)` | Same for an object field; the setter uses `setO`. |
| `propertyO$(type,var)` | Same, for a class that may be subclassed. |
| `getter$(type,var,body)`, `setter$(type,var,body)`, `property$(type,var,get,set)` | Build custom accessors. |
| `self$` | Inside a class, `self` located to this class's part of a subclass object. |
| `sizeat(type)` | `sizeof` of the object a pointer type refers to. |

Shorthand available unless you define `CODA_BASIC`: `$(name,args)`, `_` (for `self->`), `_$(name,args)` (call `Class_name(self,args)`), `_$2(name,args)`, and `ei` (for `else if`).

For the hooks (`dtor`, `itor`, `etor`, `ekeep`, `kize`, `bits`) see [OBJECT_MODEL.md](OBJECT_MODEL.md#classes-and-subclasses).

## Messages, errors and assertions

```c
void Msg_Object(Char $CONSUMED msg,int log);
Obj $NORETURN Die_Object(Char $CONSUMED msg,int trace);
```

| Macro or variable | Behavior |
| --- | --- |
| `Msg_(format,...)` | Formats a message and sends it to `Msg_hook`, or to `stderr` by default. |
| `Log_(format,...)` | Same, flagged as a log message (`log` argument is 1). |
| `Quit_(format,...)` | Sends the message and exits with status -1. |
| `Die_(format,...)` | Sends the message prefixed with `Die:` and the function name, then aborts. |
| `Assert_(expr)` | If `expr` is false, behaves like `Die_` with the message `function: !True(expr)`. Always active. |
| `Msg_hook` | `void (*)(Char msg,int log)`. Receives messages instead of `stderr`. The library frees `msg` afterwards. |
| `Die_hook` | `void (*)(Char msg,int trace)`. Receives fatal messages. The program still exits or aborts after it returns. |

## Utilities

| Macro | Equivalent |
| --- | --- |
| `cs_length(s)` | `(int)strlen(s)` |
| `cs_exact(a,b)` | `0==strcmp(a,b)` |
| `cs_strcopy(dest,src)` | `strcpy` |
| `cs_blockCopy(dest,src,n)` | `memcpy` |
| `cs_blockFill(dest,c,n)` | `memset` |
| `cs_blockZero(dest,n)` | `memset(dest,0,n)` |
| `cs_blockCmp(a,b,n)` | `memcmp` |
| `alocS(size)` | Zeroed stack memory (`alloca`). Freed when the function returns. |

```c
void Global_objectAddress(pointer address);
void Global_objectsZero(void);
```

`Global_objectAddress(&variable)` records a global object variable so `Global_objectsZero()` can release and clear it. The library registers its own registries this way. Call `Global_objectsZero()` near the end of `main` when you want a leak checker to report nothing.

## Registries and diagnostics

```c
CDictionary Class_dictionary(void);
void Class_register(Obj obj);
CDictionary Signature_dictionary(void);
void Signature_register(Char sig);
Char Signature_fromString(char *str);
CDictionary Version_dictionary(void);
void Version_register(char *key,char *text);
void codac_versions(void);
Char ToContainer(Obj container);
Char CMux_Diag(pointer cmux,char *name);
```

| Function | Behavior |
| --- | --- |
| `Class_dictionary()` | `CDictionary` of every registered class object, by name. |
| `Signature_dictionary()` | `CDictionary` of every registered signature key. |
| `Version_dictionary()` | `CDictionary` of version strings. `Version_register(key,text)` adds one. |
| `codac_versions()` | Prints the registered versions to `stderr`. |
| `ToContainer(obj)` | **New** string describing an object and everything in it, for example `(red, blue, {k : v})`. Strings come out as themselves. Can be very long. |
| `obj_(Info,obj)` | **New** one-line description of an object, from its class. |
| `Array_Diag`, `Dictionary_Diag`, `CMux_Diag` | **New** strings of internal sizes and counters, for debugging. |
| `Root_Diag` | Optional function pointer that adds text to the default `Info` of any class. |

## Deprecated and internal

| Name | Note |
| --- | --- |
| `alocC(size)` | Deprecated. Use `allocC`. |
| `etor_container`, `ekeep_container` | Used by the container classes to release and retain elements. |
| `DictHash_keyobj`, `DictHash_keydel`, `DictHash_obj4key` | The hash-mode halves of the dictionary functions. Call the `Dictionary_` functions instead. |
| `Memory_metanext` | Steps through the parts of a subclass object. |
