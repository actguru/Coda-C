# Testing

Coda-C ships with a regression suite in the `test/` folder. It builds the library source together with the tests under AddressSanitizer and UBSan, so memory errors, leaks and undefined behavior in the library itself fail the run. This page explains how to run it, how to read it, how to add tests, and how to apply the same ideas to your own code.

## Running the suite

From the repository root:

```sh
make -C test check          # build with sanitizers and run the regression tests
make -C test check-strict   # same, but any UBSan report stops the run
make -C test check-known    # also run the known-issue tests
make -C test clean          # remove test/build
```

You need gcc with the sanitizer runtime libraries (`libasan` and `libubsan`). The Makefile builds `test/test_coda.c` together with `coda-c.c` into `test/build/test_coda`.

A passing run ends like this:

```text
ok       test_root_info
ok       test_allocC
...
71 tests, 0 failed
```

A failing check prints the file, line and expression before the `not ok` line:

```text
    FAIL test/test_coda.c:231: Array_count(a) == 3  (2 vs 3)
not ok   test_array_basic
```

The program exits with a non-zero status when any test fails, so it can gate a build or a CI job.

## What the targets do

| Target | Behavior |
| --- | --- |
| `check` | Runs the regression tests. UBSan reports are collected and summarized after the run, and do not fail it. A failed check, an ASan error or a leak does. |
| `check-strict` | UBSan stops the program at its first report. Use this once a version has no open undefined-behavior findings. |
| `check-known` | Sets `CODA_KNOWN=1` to also run the `test_known_*` tests, which record behavior reported in code reviews. |

## What the suite covers

| Area | Examples of what is checked |
| --- | --- |
| Memory | Retain counts, `cleanO` scopes, `NULL` safety, `Object_Counter` balance, zero count rejection |
| `Os()` constants | Not objects, safe to keep and free, usable in containers |
| `Char` | `Char_Value`, `Char_F` and its 4,095 character cut-off, `Char_NewBlock`, `Info` |
| `Array` | Add, take, insert, replace, remove at either end and in the middle, destruction order, growth and shrink, block operations, `Array_Join`, names, nesting |
| `Dictionary` | Set, take, replace, remove, scan, keys, names, automatic hashing at 32 keys, `autoDisable`, random operations checked against a shadow table, a hasher where every key collides, switching hashers, destructors that remove other keys |
| Classes | Virtual calls, `super_`, `isa_`, `OResponds`, the class registry |
| Failure paths | Bad indexes, zero counts, `Die_`, `Quit_` and `Assert_` behavior |
| Large arrays | One million elements added and removed |

Every test also checks that `Object_Counter` returns to where it started, so a leaked object fails the test that leaked it by name.

## Reading the known-issue tests

The tests prefixed `test_known_` document behavior from code reviews that is still imperfect, such as the retain count wrap and the length of hash probe chains. They are skipped by default. Run them with `make -C test check-known`. A known-issue test that starts to fail the other way around, for example because a fix landed, is a signal to promote it to a regular test.

## Adding a test

A test is a `void` function using the check macros, registered with `RUN` in `main`.

| Macro | Checks |
| --- | --- |
| `CHECK(cond)` | the condition is true |
| `CHECK_EQ(a,b)` | two integers are equal, and prints both when not |
| `CHECK_STR(a,b)` | two strings are equal, and prints both when not |
| `RUN(fn)` | runs a test and reports it, including the leak check |
| `dies_with_signal(fn)` | runs `fn` in a child process and returns the signal that ended it, or 0 |

Use `dies_with_signal` for anything that is expected to abort, since an abort in the test process would end the whole run.

```c
static void test_array_roundtrip(void) {
	cleanO Array a=newO(Array);
	Array_add(a,Os("x"));
	Array_take(a,Char_Value("y"));
	CHECK_EQ(Array_count(a),2);
	CHECK_STR(Array_sub(a,1),"y");
	}

static void remove_past_end(void) { Array a=newO(Array); Array_removeAt(a,3); }
static void test_remove_past_end_aborts(void) { CHECK(dies_with_signal(remove_past_end)==SIGABRT); }
```

Register the tests in `main`:

```c
RUN(test_array_roundtrip);
RUN(test_remove_past_end_aborts);
```

Added to the existing suite, these two pass and the count goes from 71 to 73.

Tips:

- Declare objects with `cleanO` so the automatic leak check passes. An object you intend to keep must still be freed by the end of the test.
- Test classes defined in the test file follow the same pattern as in [USAGE.md](USAGE.md#8-defining-classes). The suite records destructor order in a global log, which is a convenient way to test cleanup.
- Keep each test independent. Do not rely on another test having run first.

## Testing your own Coda-C code

The same approach works for a project built on Coda-C:

1. Compile with `-fsanitize=address,undefined -g -O1`. Compile `coda-c.c` into the test binary so the library is instrumented too.
2. Compare `Object_Counter` before and after each unit of work. A difference is a leak, and it names the unit that leaked.
3. Call `Global_objectsZero()` at the end of `main` so the library's registries do not appear as leaks.
4. Put expected aborts behind `fork()` so one failure does not stop the others.

```sh
cc -std=gnu17 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer \
   -I/path/to/coda-c mytests.c myproject.c /path/to/coda-c/coda-c.c -lm -o mytests
./mytests
```

See [USAGE.md](USAGE.md#12-finding-leaks) for the counter pattern in a complete program.
