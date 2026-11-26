# Coda-C tests.  Run from this folder, or:  make -C tests check
#
#   make check          build with AddressSanitizer + UBSan and run the regression suite
#   make check-known    also run the known-issue tests (open review findings, expected to fail)
#   make check-strict   like check, but any UBSan report fails the run
#
# The tests compile ../coda-c.c directly so the sanitizers instrument the library itself.

CC      ?= gcc
SAN     = -fsanitize=address,undefined -fno-omit-frame-pointer
CFLAGS  = -std=gnu17 -g -O1 -Wall -Wextra $(SAN) -I..
BUILD   = build
BIN     = $(BUILD)/test_coda
UBLOG   = $(BUILD)/ubsan.log

$(BIN): test_coda.c ../coda-c.c ../coda-c.h
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) test_coda.c ../coda-c.c -o $(BIN) -lm -ldl

# UBSan reports go to a log. They are summarised but do not fail `check`, because the
# Memory_keep()/Os() misaligned read (coda-c.c, Memory_keep) is a known open finding.
check: $(BIN)
	@UBSAN_OPTIONS=print_stacktrace=0 $(BIN) 2> $(UBLOG); rc=$$?; \
	n=$$(grep -c 'runtime error' $(UBLOG)); \
	if [ "$$n" -gt 0 ]; then echo "UBSan: $$n report(s) (full log: tests/$(UBLOG))"; grep 'runtime error' $(UBLOG) | sort | uniq -c; fi; \
	grep -v 'runtime error' $(UBLOG) | grep -E 'ERROR|FAIL|SUMMARY' ; \
	exit $$rc

check-strict: $(BIN)
	@UBSAN_OPTIONS=print_stacktrace=0:halt_on_error=1 $(BIN)

check-known: $(BIN)
	@CODA_KNOWN=1 UBSAN_OPTIONS=print_stacktrace=0 ASAN_OPTIONS=detect_leaks=0 $(BIN)

clean:
	rm -rf $(BUILD)

.PHONY: check check-strict check-known clean
