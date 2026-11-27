# coda-h: the header generator

`coda-h` reads the `.c` files in a folder and writes the headers that tie them together, so you do not maintain prototypes, class declarations and signature declarations by hand. It is built by `make` and installed with the library.

## Quick start

In a folder of Coda-C source files:

```sh
coda-h -n
```

`-n` creates `coda.h` and generates the headers. Put `#include "coda.h"` first in every `.c` file. After that, run plain `coda-h` whenever the sources change.

```sh
coda-h        # regenerate; reports "No changes." when nothing differs
```

Without a `coda.h` in the folder, plain `coda-h` refuses to run:

```text
This is not a coda source folder.
Create file 'coda.h' or use option -f or -n to generate headers
```

## Options

| Option | Meaning |
| --- | --- |
| `-h` | Show usage. |
| `-n` | Create `coda.h` if it does not exist, then generate headers. |
| `-f` | Generate headers without requiring `coda.h`. |
| `-v` | Report more while working. |
| `-q` | Report less. |
| `-l` | Check header lines (`*.h`, `*.H`) for over-long lines and white space at line ends. |

## Files it writes

| File | Contents |
| --- | --- |
| `coda.h` | Created once by `-n`. Includes `coda-c.h` and `coda_generated.H`. You can edit it. |
| `coda_generated.H` | The generated declarations. Do not edit it. |
| `coda_private.H` | Declarations marked `PRIVATE`. Include it explicitly where needed. Written only when something is marked. |
| `coda_generated.c` | Generated C for `CodaStatic` and `CodaLink` uses. Written only when needed. |

## What it picks up

| In your source | In the generated header |
| --- | --- |
| A function starting in column 0 | Its prototype |
| A `static` function | Nothing (it stays private to its file) |
| A class (`CodaClass(...)`) | A `CodaClassDef(...)` line |
| A signature (`typedef ... OSig$(...)`) | `extern ConstChar sig_name;` and the call typedef |
| A line starting `//\|` | That line, with the `//\|` removed |
| A line starting `//>` | That line, placed at the top of the header |
| `PRIVATE` in a comment on a definition line | The declaration goes to `coda_private.H` instead |

Function definitions must start at the beginning of the line, with no leading white space. Only one `OSig` is allowed per line.

## Example

This two-file project defines a class, a virtual function, a public function, a private one and a static helper.

`person.c`:

```c
#include "coda.h"

struct Person_ { Char name; int age; };
CodaClassDef(Person,struct Person_,Root);

#define class Person
CodaClassZeros(itor,kize,etor,ekeep,bits);
static void $(dtor) { freeO(_ name); }
CodaClass(Person,struct Person_,Root);

//| #define PERSON_MAX_AGE 150

typedef void OSig$(greet,char *greeting);

class Person_WithName(char *name,int age) {
	class self=newO(class);
	_ name=Char_Value(name); _ age=age;
	return self;
	}

method$(void,greet,char *greeting) { printf("%s, %s (%d)\n",greeting,_ name,_ age); }

static int helper(void) { return 1; }   // static: no prototype is generated
int Person_ageNextYear(Person self) { return _ age+helper(); }


#undef class

//> #define TOP_OF_HEADER 1
int Person_internal(Person self) { return _ age*2; } // PRIVATE
```

`main.c`:

```c
#include "coda.h"
int main() {
	cleanO Person p=Person_WithName("Ada",36);
	obj_(greet,p,"Hello");
	printf("next year: %d\n",Person_ageNextYear(p));
	}
```

Run in that folder:

```sh
coda-h -n
```

It prints:

```text
Coda-C Header Generator # -h for help
Creating: coda.h
1 signature(s) defined.
1 object classes defined in coda_generated.H.
Proto types as: coda_generated.H.
Proto types as: coda_private.H.
Updated
```

`coda.h`:

```c
#pragma once

	#include <coda-c.h>
	#include "coda_generated.H"
```

`coda_generated.H`:

```c
// +------------------------------------------------------------+
// | Coda created file: coda_generated.H     @ 11/26/2026 21:36 |
// |                                                            |
// | Created with the Coda-C Header Generator                   |
// +------------------------------------------------------------+
// > person.c
 #define TOP_OF_HEADER 1
CodaClassDef(Person,struct Person_,Root); // Class: Person [Root] 
	extern ConstChar sig_greet; // Signature Key // 
// | Signatures
	typedef void OSig(greet)(Obj,char *greeting); // Call Signature // 
// | person.c
 #define PERSON_MAX_AGE 150
int main(void);
Person Person_WithName(char *name,int age);
int Person_ageNextYear(Person self);
```

`coda_private.H` holds the one declaration marked `PRIVATE`:

```c
// +------------------------------------------------------------+
// | Coda created file: coda_private.H       @ 11/26/2026 21:36 |
// |                                                            |
// | Created with the Coda-C Header Generator                   |
// +------------------------------------------------------------+
int Person_internal(Person self); // PRIVATE
```

Things to notice:

- `helper` is `static`, so it has no prototype.
- The `//|` line became `#define PERSON_MAX_AGE 150` in the header, and the `//>` line was placed at the top.
- The class and its signature are declared, so `main.c` can call `obj_(greet,p,"Hello")` without defining anything.
- `Person_internal` appears only in `coda_private.H`.

Compile and run:

```sh
cc -std=gnu17 person.c main.c -lcoda-c -lm -o demo
./demo
```

```text
Hello, Ada (36)
next year: 37
```

## Tips

- Keep `coda.h` as the first include of every source file, and list your own extra includes after it.
- Re-run `coda-h` after adding a class, a signature or a public function. A missing declaration shows up as a compile or link error.
- Commit the generated headers, or generate them as a build step. Either way, treat `coda_generated.H` as output.
- Keep source lines under 255 characters. See [LIMITS.md](LIMITS.md#known-issues).
