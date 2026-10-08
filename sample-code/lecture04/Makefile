# Lecture 4 examples. `make` builds them all; `make run` runs them in order.
CC      = gcc
CFLAGS  = -std=c17 -Wall -Wextra -Werror
PROGS   = 00-print-binary 01-structs 02-pointers 03-layout 04-nested 05-union 06-file-header \
          07-aos-soa 08-bitfields

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

# A timing program is meaningless unoptimised.
07-aos-soa: 07-aos-soa.c
	$(CC) $(CFLAGS) -O2 $< -o $@

run: all
	@for p in $(PROGS); do \
	    echo "=== $$p ==="; ./$$p; echo; \
	done

clean:
	rm -f $(PROGS)

.PHONY: all run clean
