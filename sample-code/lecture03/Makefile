# Lecture 3 examples. `make` builds them all; `make run` runs them in order.
CC      = gcc
CFLAGS  = -std=c17 -Wall -Wextra -Werror
PROGS   = 01-pointers 02-arrays 03-strings 04-2d-arrays 05-stack 06-double-pointers

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

run: all
	@for p in $(PROGS); do \
	    echo "=== $$p ==="; ./$$p; echo; \
	done

clean:
	rm -f $(PROGS)

.PHONY: all run clean
