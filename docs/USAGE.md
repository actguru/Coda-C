# Using Coda-C

This guide takes you from a first program to defining your own classes. Every code listing here is a complete program that was compiled and run, and the output shown is its real output. For function-by-function detail see [API.md](API.md); for limits you should know about before you build something large, see [LIMITS.md](LIMITS.md).

- [1. Build and install](#1-build-and-install)
- [2. A first program](#2-a-first-program)
- [3. Strings](#3-strings)
- [4. Arrays](#4-arrays)
- [5. Dictionaries](#5-dictionaries)
- [6. Ownership: who frees what](#6-ownership-who-frees-what)
- [7. Returning objects from functions](#7-returning-objects-from-functions)
- [8. Defining classes](#8-defining-classes)
- [9. Virtual calls and subclasses](#9-virtual-calls-and-subclasses)
- [10. Raw pointers: CArray, CDictionary, Pointer](#10-raw-pointers-carray-cdictionary-pointer)
- [11. Messages and errors](#11-messages-and-errors)
- [12. Finding leaks](#12-finding-leaks)
- [13. Larger projects](#13-larger-projects)

## 1. Build and install

Coda-C needs gcc or clang with GNU C extensions (`-std=gnu17`). It uses statement expressions, `$` in identifiers and zero-length arrays, so a strict ISO C mode will not work.

```sh
make                      # builds libcoda-c and the coda-h header generator
sudo make install         # installs under /usr (change with PREFIX=/usr/local)
sudo ldconfig             # Linux only: refresh the library cache
```

`make install` copies the library, `coda-c.h`, `coda-h` and the `coda-h` man page. Use `DESTDIR=/some/dir` to stage an install without touching the system.

Compile your own programs like this:

```sh
cc -std=gnu17 prog.c -lcoda-c -lm -o prog
```

On Linux, version 3.6's shared library does not record its dependency on the math library, so add `-lm` after `-lcoda-c` or the link fails with `undefined reference to llround`.

You can also skip installation and compile the library source straight into your program. This is how the samples in this guide and the test suite are built:

```sh
cc -std=gnu17 -I/path/to/coda-c prog.c /path/to/coda-c/coda-c.c -lm -o prog
```

Windows is not officially supported.

## 2. A first program

Coda-C objects are plain C pointers with a hidden header. `Array` and `Dictionary` grow as needed, and objects free themselves when you declare them with `cleanO`.

```c
#include <coda-c.h>

int main() {
	cleanO Array names=newO(Array);          // cleanO: freed automatically at end of scope
	Array_take(names,Char_Value("Ada"));     // take: the array now owns this new string
	Array_take(names,Char_Value("Grace"));
	Array_add(names,Os("Linus"));            // a string constant: nothing to allocate or free

	cleanO Dictionary ages=newO(Dictionary);
	Dict_set(ages,"Ada",Os("36"));
	Dict_set(ages,"Grace",Os("85"));

	for(int j=0;j<Array_count(names);++j) {
		char *name=Array_sub(names,j);       // objects are also plain C pointers
		char *age=Dict_sub(ages,name);
		printf("%s is %s\n",name,age ? age : "unknown");
		}

	cleanO Char all=Array_Join(names,", ");
	printf("All: %s\n",all);
	}
```

Output:

```text
Ada is 36
Grace is 85
Linus is unknown
All: Ada, Grace, Linus
```

Four things are happening here:

- `newO(Array)` creates an object of a class. It starts with a retain count of 1.
- `cleanO` releases the object when the variable goes out of scope, so there is no `free` call.
- `Array_take` adds an object and takes over your reference. `Array_add` adds an object and keeps its own.
- `Os("Linus")` is a string constant that behaves like an object. Nothing is allocated, and releasing it does nothing.

Every object is also an ordinary pointer. What `Array_sub` and `Dict_sub` return can be passed to `printf` or `strcmp` as a `char *`.

## 3. Strings

`Char` is the string class. A `Char` is a `char *` with a header in front, so it works with every C string function.

```c
#include <coda-c.h>

int main() {
	cleanO Char a=Char_Value("hello");       // copy of a C string
	cleanO Char b=Char_F("%s #%d",a,42);     // printf style
	Char c=Os("constant");                   // string constant object, never freed
	cleanO Char buf=newOC(Char,16);          // 16 zeroed bytes

	printf("a: \"%s\" size=%d count=%d kind=%s\n",a,sizeO(a),countO(a),kindO(a));
	printf("b: \"%s\"\n",b);
	printf("c: \"%s\" size=%d count=%d kind=%s\n",c,sizeO(c),countO(c),kindO(c));
	printf("isOs(a)=%d isOs(c)=%d strlen(a)=%d\n",isOs(a),isOs(c),(int)strlen(a));

	keepO(c); freeO(c);                      // harmless on constants
	snprintf(buf,sizeO(buf),"x=%d",7);
	printf("buf: \"%s\" size=%d\n",buf,sizeO(buf));
	printf("Char_Value(NULL) is %s\n",Char_Value(0) ? "an object" : "NULL");
	}
```

Output:

```text
a: "hello" size=6 count=1 kind=Char
b: "hello #42"
c: "constant" size=9 count=0 kind=ConstChar
isOs(a)=0 isOs(c)=1 strlen(a)=5
buf: "x=7" size=16
Char_Value(NULL) is NULL
```

| To get | Use |
| --- | --- |
| A copy of a C string | `Char_Value("text")` |
| A formatted string | `Char_F("%s #%d",name,n)` (4,095 characters at most) |
| A zeroed buffer of n bytes | `newOC(Char,n)` |
| A constant that is never freed | `Os("text")`, which takes a string literal only |

Declare a variable as `Char`, not `Char *`. The type already is the pointer.

Plain C strings are not objects. Never pass a bare `"literal"` or `char *` where an `Obj` is expected. Use `Os("literal")` for constants and `Char_Value(s)` for copies. See [LIMITS.md](LIMITS.md#plain-c-strings-are-not-objects).

## 4. Arrays

An `Array` holds objects and retains them, so everything you add is released when the array is destroyed.

```c
#include <coda-c.h>

static void show(Array a,char *label) {
	cleanO Char s=ToContainer(a);            // diagnostic text for any container
	printf("%-14s %s\n",label,s);
	}

static int compare(const void *aa,const void *bb) { return strcmp(*(char **)aa,*(char **)bb); }

int main() {
	cleanO Array a=newO(Array);
	Array_add(a,Os("b")); Array_add(a,Os("c"));
	show(a,"add:");
	Array_insertAt(a,0,Os("a"));                  // at the front
	Array_insertAt(a,Array_count(a),Os("d"));     // index == count appends
	show(a,"insertAt:");
	Array_replaceAt(a,1,Os("B"));
	show(a,"replaceAt:");
	Array_removeAt(a,2);
	show(a,"removeAt(2):");
	printf("removeLast() returned %d\n",Array_removeLast(a));
	show(a,"after:");

	Obj x=Os("x");
	Array_add(a,x); Array_add(a,x);
	printf("removeObject(x) removed %d\n",Array_removeObject(a,x));
	printf("Array_sub(a,99) is %s\n",Array_sub(a,99) ? "an object" : "NULL");

	Array_removeAll(a);
	printf("after removeAll: count=%d, removeLast()=%d\n",Array_count(a),Array_removeLast(a));

	Array_set_name(a,"fruit");
	Array_take(a,Char_Value("pear")); Array_take(a,Char_Value("apple")); Array_take(a,Char_Value("fig"));
	qsort(Array_rawAddress(a),Array_count(a),sizeof(void *),compare);   // sort in place
	cleanO Char joined=Array_Join(a," < ");
	printf("%s: %s\n",Array_name(a),joined);
	}
```

Output:

```text
add:           (b, c)
insertAt:      (a, b, c, d)
replaceAt:     (a, B, c, d)
removeAt(2):   (a, B, d)
removeLast() returned 1
after:         (a, B)
removeObject(x) removed 2
Array_sub(a,99) is NULL
after removeAll: count=0, removeLast()=0
fruit: apple < fig < pear
```

Points worth noting:

- Indexes passed to `Array_insertAt`, `Array_replaceAt`, `Array_removeAt` and `Array_removeBlock` must be valid. An invalid index aborts the program. The exception is `Array_insertAt` with an index equal to the count, which appends.
- `Array_sub` is forgiving. An out-of-range index returns `NULL`.
- `Array_removeLast` returns `false` on an empty array instead of aborting.
- `Array_removeObject` compares by address and removes every match.
- An array accepts `NULL` elements.
- `Array_rawAddress` exposes the internal pointer vector, which is handy for `qsort`. The address is invalid once the array changes.
- Inserting or removing at either end is cheap. The middle costs a move of the elements behind it.

## 5. Dictionaries

A `Dictionary` maps string keys to objects. Keys are copied, and values are retained.

```c
#include <coda-c.h>

int main() {
	cleanO Dictionary d=newO(Dictionary);
	Dictionary_set_name(d,"config");
	Dict_set(d,"host",Os("localhost"));
	Dict_set(d,"port",Os("8080"));
	Dict_take(d,"user",Char_F("user-%d",7));   // take: the dictionary owns the new string
	printf("count=%d host=%s\n",Dictionary_count(d),(char *)Dict_sub(d,"host"));

	Dict_set(d,"port",Os("9090"));             // same key: replaces the value
	printf("port=%s count=%d\n",(char *)Dict_sub(d,"port"),Dictionary_count(d));
	printf("missing key gives %s\n",Dict_sub(d,"nope") ? "an object" : "NULL");

	cleanO Array tags=newO(Array);             // containers nest
	Array_add(tags,Os("a")); Array_add(tags,Os("b"));
	Dict_set(d,"tags",tags);
	cleanO Char text=ToContainer(d);
	printf("%s\n",text);

	printf("keys, insertion order:");
	cleanO Pointer keys=Dictionary_AllKeys(d);
	for(int j=0;j<Pointer_count(keys);++j) printf(" %s",(char *)((Pointer)keys)[j]);
	printf("\nscan, newest first:  ");
	for(Keyword k=Dictionary_scan(d);k;k=Dictionary_next(k)) printf(" %s",k->word);
	printf("\n");

	int removed=Dictionary_removeKey(d,"user");
	int again=Dictionary_removeKey(d,"user");
	printf("removeKey(user)=%d, again=%d\n",removed,again);
	Dict_set(d,"port",0);                      // a NULL value also removes the key
	printf("count now %d\n",Dictionary_count(d));

	cleanO Dictionary big=newO(Dictionary);    // switches to a hash table at 32 keys
	for(int j=0;j<1000;++j) { char key[32]; snprintf(key,sizeof(key),"key_%d",j); Dict_take(big,key,Char_F("%d",j*j)); }
	printf("big: count=%d key_30=%s key_999=%s\n",Dictionary_count(big),(char *)Dict_sub(big,"key_30"),(char *)Dict_sub(big,"key_999"));
	}
```

Output:

```text
count=3 host=localhost
port=9090 count=3
missing key gives NULL
<config>{tags : (a, b), user : user-7, port : 9090, host : localhost}
keys, insertion order: host port user tags
scan, newest first:   tags user port host
removeKey(user)=1, again=0
count now 2
big: count=1000 key_30=900 key_999=998001
```

- `Dict_set` replaces an existing key's value, and setting a key to `NULL` removes it.
- `Dict_sub` returns `NULL` for a missing key.
- `Dictionary_AllKeys` returns a new `Pointer` object of the key strings, in the order they were added, or `NULL` for an empty dictionary. The key strings belong to the dictionary.
- `Dictionary_scan` and `Dictionary_next` walk the entries from the newest to the oldest. Do not add or remove keys while you walk.
- A dictionary is a linked list until it reaches 32 keys, then it builds a hash index on its own. Lookups stay fast with no action from you. See [`Dictionary_autoDisable`](API.md#dictionary-cdictionary-and-keyword) to turn this off.

## 6. Ownership: who frees what

Every object has a retain count, and it starts at 1. Three rules cover everything:

1. **Whoever creates an object owns that reference.** `newO`, `Char_Value`, `Char_F`, `Array_Join`, `Dictionary_AllKeys`, `ToContainer` and the other constructors all hand you a new reference.
2. **Containers retain what you add and set, and release what they remove.** The `take` variants (`Array_take`, `Dict_take`) take over your reference instead, so a freshly created object goes in with no extra release.
3. **Release what you own exactly once**, with `freeO(obj)` or by declaring the variable `cleanO`.

```c
#include <coda-c.h>

static Char make(void) { return Char_F("made"); }    // returns a new object: the caller owns it

static void show(char *label,Obj o) { printf("%-30s count=%d\n",label,countO(o)); }

int main() {
	huge before=Object_Counter;
	{
	Char s=make();                    show("new object:",s);
	cleanO Array a=newO(Array);
	Array_add(a,s);                   show("after Array_add (retains):",s);
	freeO(s);                         show("after our own freeO:",s);

	Char t=make();
	Array_take(a,t);                  show("after Array_take (consumes):",t);

	Char owner=0;                     // a variable that owns what it points at
	Char x=Char_Value("first");
	setO(owner,x);                    // keepO(x), freeO(old owner), owner=x
	freeO(x);                         // drop our own reference; owner still holds it
	show("held by owner:",owner);
	setO_take(owner,Char_Value("second"));   // frees "first", takes ownership of "second"
	printf("owner is now \"%s\"\n",owner);
	freeO(owner);
	}
	printf("live objects added by this code: %lld\n",(long long)(Object_Counter-before));
	}
```

Output:

```text
new object:                    count=1
after Array_add (retains):     count=2
after our own freeO:           count=1
after Array_take (consumes):   count=1
held by owner:                 count=1
owner is now "second"
live objects added by this code: 0
```

| Call | Effect on retain count |
| --- | --- |
| `keepO(obj)` | +1, returns `obj` |
| `freeO(obj)` | -1, destroys the object at 0 |
| `Array_add`, `Dict_set`, `setO` | the container or variable keeps its own +1 |
| `Array_take`, `Dict_take`, `setO_take` | takes your reference, no change for you to undo |

`freeO(NULL)` and `keepO(NULL)` are safe, and so is `freeO` on an `Os` constant.

Variables declared with `cleanO` are `const`, so you cannot reassign them. For a variable that changes, use a plain variable with `setO` / `setO_take`, and call `freeO` when you finish. For raw `malloc`-style memory the matching tools are `allocC`, `freeC` and `cleanC`.

## 7. Returning objects from functions

A function that returns a new object hands its reference to the caller. If you build the object in a `cleanO` variable, `cleanO` releases your reference at the closing brace, so take an extra reference for the caller with `keepO` in the `return`:

```c
#include <coda-c.h>

// Returns a new array of the words in text. The caller owns the result.
static Array split_words(char *text) {
	cleanO Array words=newO(Array);       // released automatically when we leave the function...
	char *start=text;
	for(char *p=text;;++p) {
		if (*p==' ' || *p==0) {
			if (p>start) Array_take(words,Char_F("%.*s",(int)(p-start),start));
			start=p+1;
			}
		if (*p==0) break;
		}
	return keepO(words);                  // ...so keepO first: the caller gets its own reference
	}

int main() {
	huge before=Object_Counter;
	{
	cleanO Array words=split_words("the quick  brown fox");
	cleanO Char joined=Array_Join(words,"|");
	printf("%d words: %s\n",Array_count(words),joined);
	}
	printf("live objects added by this code: %lld\n",(long long)(Object_Counter-before));
	}
```

Output:

```text
4 words: the|quick|brown|fox
live objects added by this code: 0
```

The last line confirms nothing leaked.

## 8. Defining classes

A class is a C struct plus a few macro lines. The program at the end of this section defines a base class, `Account`, with a destructor and properties. It also defines a subclass, `Savings`, which [section 9](#9-virtual-calls-and-subclasses) explains.

The pattern, in order:

1. Declare the struct and forward-define the class with `CodaClassDef` if the destructor needs `self`.
2. `#define class Name` so the short macros know which class you are in.
3. List the hooks you are **not** defining in `CodaClassZeros(...)`. Use `CodaClassZerosC()` for "none of them".
4. Define the hooks you do want, such as `$(dtor)`.
5. `CodaClass(Name,Storage,Super)` finishes the definition. Use `Root` as the super for a base class.
6. Add properties, constructors and methods.
7. `#undef class`.

| Hook | Purpose |
| --- | --- |
| `dtor` | destroys the object's own fields |
| `itor` | initializes a new object (memory starts zeroed) |
| `etor`, `ekeep` | for containers: release and retain an element |
| `kize` | makes `newOC(class,n)` reserve `n` extra bytes instead of making an array of objects |
| `bits` | marks special classes (see [OBJECT_MODEL.md](OBJECT_MODEL.md#class-bits)) |

Inside a class, `$(name,args)` declares `Class_name(Class self,args)` and `_` stands for `self->`. So `_ owner` is `self->owner`.

The full program:

```c
#include <coda-c.h>

typedef Char OSig$(describe);                 // a virtual function: Char describe(Obj self)

// ---- Account: a class with an object field, a destructor and properties ----
struct Account_ { Char owner; double balance; };
CodaClassDef(Account,struct Account_,Root);   // forward definition, needed because dtor uses 'self'

#define class Account
CodaClassZeros(itor,kize,etor,ekeep,bits);    // dtor is defined below, so it is not in this list
static void $(dtor) { freeO(_ owner); }
CodaClass(Account,struct Account_,Root);

propertyO$(Char,owner);                       // Account_owner(), Account_set_owner()
property$$(double,balance);                   // Account_balance(), Account_set_balance()

class Account_New(char *owner,double balance) {
	class self=newO(class);
	_ owner=Char_Value(owner);
	_ balance=balance;
	return self;
	}

method$(Char,describe) { return Char_F("%s has %.2f",_ owner,_ balance); }
#undef class

// ---- Savings: a subclass that adds a field and extends describe ----
struct Savings_ { double rate; };

#define class Savings
CodaClassZerosC();
CodaClass(Savings,struct Savings_,Account);

class Savings_New(char *owner,double balance,double rate) {
	class self=newO(class);
	cleanO Char name=Char_Value(owner);
	Account_set_owner((Obj)self,name);          // inherited property; cast to Obj; the setter retains name
	Account_set_balance((Obj)self,balance);
	_ rate=rate;
	return self;
	}

method$(Char,describe) {
	cleanO Char base=super_(describe,self);   // call Account's version first
	return Char_F("%s at %.1f%%",base,_ rate*100);
	}
SubClassOk();                                 // compile-time alignment check
#undef class

int main() {
	cleanO Obj a=Account_New("Ada",120.5);
	cleanO Obj s=Savings_New("Grace",1000,0.03);

	Obj accounts[]={a,s};
	for(int j=0;j<2;++j) {
		cleanO Char text=obj_(describe,accounts[j]);   // virtual call
		printf("%-8s %s\n",kindO(accounts[j]),text);
		}

	Account_set_balance(s,Account_balance(s)+250);     // inherited accessors work on the subclass
	printf("Grace now has %.2f\n",Account_balance(s));
	printf("isa Account: a=%d s=%d   isa Savings: a=%d s=%d\n",
		isa_(a,Account),isa_(s,Account),isa_(a,Savings),isa_(s,Savings));
	printf("responds to describe: %s\n",OResponds(a,describe) ? "yes" : "no");
	printf("superclass segment of s: %s\n",kindO(superO(s)));
	}
```

Properties generate getters and setters:

| Macro | Generates | Use for |
| --- | --- | --- |
| `property$_(type,var)` | `Class_var()`, `Class_set_var()` | a plain value, class never subclassed |
| `property$$(type,var)` | same | a plain value, class may be subclassed |
| `propertyO_(type,var)` | same, setter uses `setO` | an object field, never subclassed |
| `propertyO$(type,var)` | same, setter uses `setO` | an object field, may be subclassed |

Use the `$` forms (`property$$`, `propertyO$`) for any class that other classes will extend. They locate the right part of a subclass object before touching the field.

Add `SubClassOk();` at the end of every subclass. It is a compile-time check that the subclass storage is a multiple of the pointer size, which keeps object headers aligned.

## 9. Virtual calls and subclasses

A virtual function needs a **signature**: a typedef that fixes the return type and arguments.

```c
typedef Char OSig$(describe);                 // Char describe(Obj self)
typedef void OSig$(greet,char *greeting);     // void greet(Obj self,char *greeting)
```

`method$(type,name,args)` defines an implementation for the current class and registers it at program start. Call it with `obj_(name,object,args)`. A subclass can call the version it overrides with `super_(name,self,args)`.

The program from the previous section produces:

```text
Account  Ada has 120.50
Savings  Grace has 1000.00 at 3.0%
Grace now has 1250.00
isa Account: a=1 s=1   isa Savings: a=0 s=1
responds to describe: yes
superclass segment of s: Account
```

What this shows:

- `obj_(describe,x)` chose `Account`'s or `Savings`'s version by the object's real class.
- `Savings` extended the base version with `super_(describe,self)`.
- Inherited accessors (`Account_balance`, `Account_set_balance`) work on a `Savings` object. When you call one with a subclass pointer from inside the subclass's own code, cast to `Obj` as the repository's `shapes.c` example does.
- `isa_(obj,Class)` tests inheritance, `kindO(obj)` gives the class name, `superO(obj)` gives the superclass part of the object, and `OResponds(obj,name)` tests for an implementation.

Calling `obj_` with a signature the object does not implement aborts with a message such as `Class:ConstChar does not respond to: describe`. Check with `OResponds` first when you are not sure.

A signature is identified by its key constant, so define it once and declare it `extern ConstChar sig_name;` in other files. The [`coda-h`](CODA-H.md) generator does this for you.

### Basic mode

If the short macros (`$`, `_`, `_$`, `_$2` and `ei`) clash with your own code, define `CODA_BASIC` before including the header. Everything else keeps working.

```c
#define CODA_BASIC 1                          // leaves out the short $ _ and ei macros
#include <coda-c.h>

#define class Counter
CodaClassZerosC();
CodaClass(Counter,int,Root);                  // storage is a plain int
Counter Counter_Value(int start) { Counter self=newO(Counter); *self=start; return self; }
#undef class

int main() {
	cleanO Counter c=Counter_Value(41);
	++(*c);
	printf("%s holds %d\n",kindO(c),*c);
	}
```

```text
Counter holds 42
```

## 10. Raw pointers: CArray, CDictionary, Pointer

`CArray` and `CDictionary` have the same functions as `Array` and `Dictionary` but hold plain pointers. They never retain, release or free the things they hold. Use them for any pointer that is not a Coda-C object. A `Pointer` object is a counted block of pointers, good for passing a result set around.

```c
#include <coda-c.h>

int main() {
	int x=1,y=2;

	cleanO CArray raw=newO(CArray);           // holds raw pointers: nothing is retained or freed
	Array_add(raw,&x); Array_add(raw,&y);
	printf("CArray count=%d first=%d\n",Array_count(raw),*(int *)Array_sub(raw,0));

	cleanO CDictionary lookup=newO(CDictionary);   // same idea for a dictionary
	Dict_set(lookup,"x",&x);
	printf("CDictionary x=%d\n",*(int *)Dict_sub(lookup,"x"));

	void *list[]={&x,&y};
	cleanO Pointer p=Pointer_NewBlock(0,2,list);   // a Pointer object copies a block of pointers
	printf("Pointer count=%d\n",Pointer_count(p));

	char *words[]={"apple","grape",0,"foo",0};
	printf("Pointer_length=%d\n",Pointer_length(words));   // counts up to the first NULL
	}
```

Output:

```text
CArray count=2 first=1
CDictionary x=1
Pointer count=2
Pointer_length=2
```

## 11. Messages and errors

`Msg_` and `Log_` format a message like `printf` and send it to `stderr`, or to a function you install in `Msg_hook`. The library frees the message after the hook returns, so the hook must not free it.

`Quit_` prints a message and exits with status -1 (255 in a shell). `Die_` prefixes the message with the function name and aborts. `Assert_(condition)` is a check that is always active, even with `-DNDEBUG`, and aborts with the failed expression.

```c
#include <coda-c.h>

static void my_msg(Char msg,int log) {        // the library frees msg after the hook returns
	printf("[%s] %s\n",log ? "log" : "msg",msg);
	}

int main(int argc,char **argv) {
	Msg_hook=my_msg;
	Msg_("hello %d",1);
	Log_("logged %s","this");
	Msg_hook=0;                               // back to stderr

	if (argc>1 && cs_exact(argv[1],"quit")) Quit_("quitting: %s","bye");
	if (argc>1 && cs_exact(argv[1],"die"))  Die_("fatal: %s","boom");
	if (argc>1 && cs_exact(argv[1],"assert")) Assert_(1==2);
	printf("finished normally\n");
	}
```

Running it with no argument prints:

```text
[msg] hello 1
[log] logged this
finished normally
```

With an argument:

| Run | stderr | Result |
| --- | --- | --- |
| `./a.out quit` | `quitting: bye` | exits with status 255 |
| `./a.out die` | `Die:main: fatal: boom` | aborts (SIGABRT) |
| `./a.out assert` | `main: !True(1==2)` | aborts (SIGABRT) |

`Die_hook` works like `Msg_hook` for fatal messages. The program still exits or aborts after the hook returns.

## 12. Finding leaks

`Object_Counter` is the number of live objects. Compare it before and after a block to check the block is balanced. `Global_objectsZero()` releases the library's own registries so that `Object_Counter` returns to 0 and leak checkers such as AddressSanitizer or valgrind stay quiet.

```c
#include <coda-c.h>

int main() {
	huge before=Object_Counter;
	{
	cleanO Array a=newO(Array);
	Array_take(a,Char_Value("x"));
	Array_take(a,Char_Value("y"));
	printf("inside the scope: %lld live objects added\n",(long long)(Object_Counter-before));
	}
	printf("after the scope:  %lld live objects added\n",(long long)(Object_Counter-before));

	Global_objectsZero();                     // free the library's own registries
	printf("after teardown:   Object_Counter=%lld\n",(long long)Object_Counter);
	}
```

Output:

```text
inside the scope: 3 live objects added
after the scope:  0 live objects added
after teardown:   Object_Counter=0
```

The [test suite](TESTING.md) applies this check to every test automatically.

## 13. Larger projects

Once a program spans several `.c` files, keeping prototypes, class definitions and signature declarations in sync by hand gets tedious. The `coda-h` tool generates them. See [CODA-H.md](CODA-H.md).

To study complete programs, read the files in the repository's `examples/` folder, described in [EXAMPLES.md](EXAMPLES.md).
