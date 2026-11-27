# Coda-C documentation

Coda-C is an object system for C: retain-counted objects, classes with inheritance and virtual calls, and a dynamic `Array` and `Dictionary`, all written in plain C with no preprocessor step. It is a shared library (`libcoda-c`), one header (`coda-c.h`) and a header generator (`coda-h`).

This documentation describes Coda-C 3.6.

## Where to start

| If you want to... | Read |
| --- | --- |
| Install it and write your first program | [USAGE.md](USAGE.md) |
| Look up a function or macro | [API.md](API.md) |
| Understand how objects, classes and calls work | [OBJECT_MODEL.md](OBJECT_MODEL.md) |
| Generate headers for a multi-file project | [CODA-H.md](CODA-H.md) |
| Know the limits before you rely on it | [LIMITS.md](LIMITS.md) |
| Run or extend the test suite | [TESTING.md](TESTING.md) |
| Read working programs | [EXAMPLES.md](EXAMPLES.md) |

## Thirty-second tour

```c
#include <coda-c.h>

int main() {
	cleanO Array names=newO(Array);          // freed automatically at end of scope
	Array_take(names,Char_Value("Ada"));     // the array owns the new string
	Array_add(names,Os("Linus"));            // a string constant, nothing to free
	printf("%d names, first is %s\n",Array_count(names),(char *)Array_sub(names,0));
	}
```

The ideas behind it:

- **Objects are pointers.** A `Char` is a `char *` you can pass to `printf`. Each object has a hidden header with its class and a retain count.
- **Ownership is explicit.** Creating an object gives you one reference. Containers retain what you add. `freeO` or `cleanO` releases yours.
- **Classes are C structs.** A few macros add a destructor, properties, subclasses and virtual functions that you call with `obj_`.
- **Failures abort.** Misuse such as an invalid index stops the program with a message instead of returning an error code. [LIMITS.md](LIMITS.md) lists them.

## Documents

- [USAGE.md](USAGE.md): a guided tour from build to defining classes, with runnable programs.
- [API.md](API.md): reference for every public function, macro and variable, grouped by area.
- [OBJECT_MODEL.md](OBJECT_MODEL.md): memory layout, class records, subclass layout, signatures and dispatch, container internals.
- [CODA-H.md](CODA-H.md): what `coda-h` generates, with a worked example.
- [LIMITS.md](LIMITS.md): measured limits, which situations abort, threading, and known issues.
- [TESTING.md](TESTING.md): running the suite, adding tests, testing your own code.
- [EXAMPLES.md](EXAMPLES.md): the eight programs in `examples/` with their output.

## Conventions used in these documents

- Code listings in USAGE.md and CODA-H.md are complete programs. They were compiled with `-std=gnu17` and run under AddressSanitizer and UBSan, and the output shown is real output.
- Function declarations in API.md were copied from `coda-c.h`.
- "New" means a function returns an object with a retain count of 1 that you must release. "Retains" and "takes" describe how a function treats the references you hand it.
- Sizes and limits are for a 64-bit system.

## License and credits

Coda-C is written by Stephen M. Jones and released under the GNU Affero General Public License, version 3 or later. See `LICENSE` in the repository root. The PDF guide and the man pages in the repository (`Coda-C.pdf`, `Coda-C_MAN.pdf`) are the author's original documentation.
