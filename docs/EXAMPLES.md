# Examples

The `examples/` folder holds eight small programs. Each ends with a comment showing its expected output. This page says what each one teaches, and shows the output from a real run on 3.6.

## Building and running

With the library installed:

```sh
cc -std=gnu17 examples/shapes.c -lcoda-c -lm -o shapes
./shapes
```

Without installing, compile the library source along with the example:

```sh
cc -std=gnu17 -I. examples/shapes.c coda-c.c -lm -o shapes
```

`sortlines` and `wordcount` read standard input.

| Example | Teaches | Read it after |
| --- | --- | --- |
| `aperson.c` | A class with a destructor, properties and a constructor | [USAGE §8](USAGE.md#8-defining-classes) |
| `xyzperson.c` | A subclass overriding a virtual function and calling `super_` | [USAGE §9](USAGE.md#9-virtual-calls-and-subclasses) |
| `shapes.c` | Inheritance and polymorphism with `double` results | [USAGE §9](USAGE.md#9-virtual-calls-and-subclasses) |
| `containers.c` | Mixed containers, `ToContainer`, `Object_Counter` | [USAGE §12](USAGE.md#12-finding-leaks) |
| `jsonout.c` | Adding a new virtual function to the library's own classes | [OBJECT_MODEL](OBJECT_MODEL.md#virtual-calls) |
| `wordcount.c` | A `Dictionary` counting words with a custom `Int4` class | [USAGE §5](USAGE.md#5-dictionaries) |
| `sortlines.c` | An `Array` of strings sorted with `qsort` on `Array_rawAddress` | [USAGE §4](USAGE.md#4-arrays) |
| `codainfo.c` | Listing registered classes, signatures and versions | [OBJECT_MODEL](OBJECT_MODEL.md#class-registration) |

## aperson.c

Defines `Person` with a `Char` name and an `int` age. The destructor frees the name, and `Person_WithName` is a constructor.

```text
Hello, my name is Alice and I am 30 years old.
Hello, my name is Alice and I am 31 years old.
_ name was destroyed!
```

## xyzperson.c

`XyzShoutingPerson` extends `XyzPerson`. Its `saySomething` upper-cases the greeting and passes it to the superclass's version with `super_`.

```text
Hello, world!
HELLO, WORLD!
```

## shapes.c

`Circle` and `Rectangle` inherit from `Shape` and each implement `calculate_area` and `calculate_perimeter`. One function prints the details of any `Shape` by calling those through `obj_`.

```text
---       Circle  Details  Color:Blue      Area: 78.54  Perimeter: 31.42  ---
---    Rectangle  Details  Color:Green     Area: 24.00  Perimeter: 20.00  ---
```

## containers.c

Puts a string, string constants, a `NULL`, an object array, a `Void` block and a dictionary into one `Array`, prints the whole thing with `ToContainer`, and shows that `Object_Counter` returns to its starting value.

```text
Object Count: 7
Array list:  (red, blue, green, <Null>, Crazy[7], Void[5], {Key2 : 7.89, Key1 : 3.14})
list info.:  Array[7] <Dynamic object array>
Object Count: 7
Final  Count: 0
```

## jsonout.c

Defines a `JSON_out` signature and implements it for `Dictionary`, `Array` and `Char`, the library's own classes. A nested employee record then prints itself as JSON. The string output is simple and does not escape quotes or control characters.

```text
{
    "name" : "Jane Doe",
    "email" : "jane.doe@example.com",
    "roles" : [ "Admin", "User" ],
    "address" : {
        "city" : "Atlanta",
        "zipcode" : "30301"
        }
    }
```

## wordcount.c

Reads words from standard input and counts them in a `Dictionary` whose values are a small `Int4` class. Run with `printf 'the quick brown fox\njumps over the quick dog\n' | ./wordcount`. Words appear newest first, the order `Dictionary_scan` walks.

```text
dog=1  over=1  jumps=1  fox=1  brown=1  quick=2  the=2  
```

## sortlines.c

Reads lines into an `Array` and sorts the underlying pointer vector with `qsort`. Run with `printf 'pear\napple\nfig\n' | ./sortlines`.

```text
apple
fig
pear
```

## codainfo.c

Lists every registered class with its `Info` text, every registered signature, and the version registry. Its output is about twenty lines and changes as you add classes and signatures, so run it to see your own program's registry.
