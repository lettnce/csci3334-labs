# Lab 2 Extra Practice: test your C against the packet's assembly.
#
#   make test           test all eighteen functions in mine.c
#   make test F=fib     test just one
#   make clean
#
# Needs Linux x86-64, the same machine you use for Lab 2.

CC     = gcc
# -fwrapv: signed overflow wraps, the way the assembly does, so a function
# like power() can be compared on large inputs.
# -Wno-unused-parameter: the unfinished stubs in mine.c ignore their arguments.
CFLAGS = -std=c17 -O1 -Wall -Wextra -Wno-unused-parameter -fwrapv

test: tests
	@./tests $(F)

tests: tests.c mine.c listings.s
	@uname -sm | grep -q 'Linux x86_64' || { \
	    echo "This needs Linux x86-64 (the Lab 2 machine). This is: $$(uname -sm)"; exit 1; }
	$(CC) $(CFLAGS) -o $@ tests.c mine.c listings.s

clean:
	rm -f tests

.PHONY: test clean
