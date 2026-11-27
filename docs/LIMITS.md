# Limits, behavior to know, and known issues

What Coda-C 3.6 will and will not do, with the numbers that were measured. The [Known issues](#known-issues) section describes the library as of this version and may be out of date after later releases.

## Summary

| Area | Limit or rule |
| --- | --- |
| Single object size | Under 8,388,607 bytes when created with `newO`/`newOC` |
| Retain count | 65,535 at most, then it wraps |
| `Char_F` result | 4,095 characters, longer output is cut off |
| `Array` / `Dictionary` size | Not limited by the object size limit |
| Strings passed as objects | Must be `Char` objects or `Os()` constants |
| Compiler | gcc or clang with `-std=gnu17` |
| Threads | Objects and containers are not synchronized |

## Plain C strings are not objects

`Obj` is `void *`, so the compiler will not stop you from passing a plain `char *` where an object is expected. Do not. Every object function reads two bytes in front of the pointer to tell objects from constants, and a plain string has something else there. The result is undefined behavior.

| You have | Do this |
| --- | --- |
| A string literal | `Os("literal")` |
| A `char *` you want to keep | `Char_Value(s)` |
| A `char *` you only want to use briefly | Use it as a `char *`. It becomes a problem only when you hand it to an object function. |

Going the other way is always fine: any `Char` or `Os` constant is a valid `char *`.

`Os` takes a string literal, not a variable. Do not write to an `Os` constant.

## Object size

`newO`, `newOC` and the class constructors abort when one object's storage would reach 8,388,607 bytes. Measured on 3.6:

| Call | Result |
| --- | --- |
| `newOC(Char,8000000)` | works, size 8,000,000 |
| `newOC(Char,9000000)` | aborts: `Memory_newOOR: !True(size+extra<0x7FFFFF)` |

The limit is per object. An `Array` keeps its elements in separate memory, so it can hold millions of elements: arrays of 2,000,000 elements were created and cleared in testing. The limit matters for one large `Char`, `Pointer` or `kize` object.

## Retain counts

The retain count is 16 bits. Counting from 1, the 65,535th `keepO` call takes it past 65,535 and it becomes 0. A count of 0 means "never freed", so from then on `freeO` does nothing and the object leaks. There is no warning. This was measured on 3.6: the count read 0 after 65,535 calls.

If one object may be shared by more than 65,534 owners, give each group its own object or keep your own counter.

## Errors abort the program

Coda-C reports misuse by aborting, not by returning an error code:

| Situation | Result |
| --- | --- |
| `Array_insertAt`, `Array_replaceAt`, `Array_removeAt`, `Array_removeBlock` with an invalid index | aborts |
| `newOC(class,0)` for a class without `CodaClass_Zerok()` | aborts |
| `obj_` with a signature the object does not implement | aborts, `does not respond to` |
| `newO`/`newOC` for an object over the size limit | aborts |
| `Assert_` failing | aborts |
| Out of memory in `allocC` | aborts |

These are gentler:

| Situation | Result |
| --- | --- |
| `Array_sub` past either end | `NULL` |
| `Array_removeLast` on an empty array | `false` |
| `Dict_sub` for a missing key | `NULL` |
| `Dictionary_removeKey` for a missing key | `false` |
| `freeO(NULL)`, `keepO(NULL)` | no effect |
| `Array_count(NULL)`, `Dictionary_count(NULL)` | `0` |

`Die_` and `Assert_` abort, so the process ends with `SIGABRT`. `Quit_` exits with status -1 (255). `Die_hook` and `Msg_hook` let you capture the message, but they do not stop the program from ending.

`Assert_` is always on. Building with `-DNDEBUG` does not remove it.

## Strings

- `Char_F` formats into a 4,095 character buffer and cuts longer output off without an error. Build long strings with `newOC(Char,n)` and `snprintf`, or join pieces with `Array_Join`.
- `Char_NewBlock` copies exactly the bytes you give it and does not add a terminator.
- `Array_Join` considers only `Char` elements and silently skips the rest.

## Threads

The retain count and `Object_Counter` are atomic, but sequences such as "check the count, then increment" are not, and `Array`, `Dictionary` and the class and signature registries have no locks. Treat objects as belonging to one thread at a time, or add your own locking around shared ones.

## Iteration order

`Dictionary_AllKeys` returns keys in the order they were added. `Dictionary_scan` and `Dictionary_next` walk newest to oldest. Hash mode does not change either order. Do not add or remove keys while walking.

## Building

- Compilers: gcc or clang with `-std=gnu17`. The header uses statement expressions, `$` in identifiers and zero-length arrays.
- Linking: on Linux the 3.6 shared library needs `-lm` on the link line (see [USAGE.md](USAGE.md#1-build-and-install)).
- Windows is not officially supported, and the Makefile notes install problems there.

## Naming

Class functions use the name `Class_function`, so class and function names share the global namespace of your program. The macro `class` is defined only between `#define class Name` and `#undef class`, so keep those pairs around every class definition. Use `CODA_BASIC` if `$`, `_` or `ei` collide with your code.

## Known issues

As of version 3.6. These were found by running the library under AddressSanitizer and UBSan and by reading the source.

| Issue | Effect | Workaround |
| --- | --- | --- |
| Subclass storage not a multiple of 8 bytes | A subclass whose own storage is, for example, a `short` puts the next object header at a misaligned address. UBSan reports it, and stricter CPUs may fault. | Add `SubClassOk();` after every subclass, which turns the mistake into a compile error. Pad the storage to a multiple of 8 bytes. |
| `alocO` has no size check | `alocO(20000000)` succeeds but `sizeO` reports 3,222,784, because the 24-bit size field wraps at 16,777,216. | Keep `alocO` objects under 8 MB, or use `allocC` for large blocks. |
| Retain count wraps silently | See [Retain counts](#retain-counts). | Keep sharing under 65,535. |
| `coda-h` copies source lines into fixed buffers | A very long source line can overflow a buffer or abort the generator. | Keep source lines under 255 characters in the files `coda-h` reads. |
