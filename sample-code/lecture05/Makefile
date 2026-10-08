# Lecture 5 examples. `make` builds them all; `make run` runs them in order.
CC      = gcc
CFLAGS  = -std=c17 -Wall -Wextra -Werror
PROGS   = 00-rewind 01-wrap 01b-promotion 02-signed 03-intmin 04-average 05-compare 06-alloc \
          07-shift 08-quantize 09-ubsan

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

# 09 is deliberately broken. `make ubsan` builds the version that says so.
ubsan: 09-ubsan.c
	$(CC) $(CFLAGS) -O2 -fsanitize=undefined $< -o 09-ubsan-san
	@echo "built 09-ubsan-san — try: ./09-ubsan-san 2147483647 1"

run: all
	@for p in $(PROGS); do \
	    echo "=== $$p ==="; \
	    if [ "$$p" = "09-ubsan" ]; then ./$$p 2147483647 1; else ./$$p; fi; \
	    echo; \
	done

clean:
	rm -f $(PROGS) 09-ubsan-san

.PHONY: all run ubsan clean
