# Lecture 5b examples. `make` builds them all; `make run` runs them in order.
CC      = gcc
CFLAGS  = -std=c17 -Wall -Wextra -Werror
LDLIBS  = -lm
PROGS   = 00-float-bits 01-bits 02-spacing 03-not-point-one 04-rounding 05-special \
          06-order 07-bf16

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

run: all
	@for p in $(PROGS); do \
	    echo "=== $$p ==="; ./$$p; echo; \
	done

clean:
	rm -f $(PROGS)

.PHONY: all run clean
