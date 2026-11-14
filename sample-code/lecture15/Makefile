# Lecture 15 examples.
#
#   make          build them for THIS machine
#   make run      run them all in order (a few seconds)
#   make clean
#
# Everything builds and runs on Linux and macOS. The numbers depend on the
# machine; the slides show a Linux laptop with an AMD Ryzen 7 8845HS.

CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -O1
PROGS   = 01-conflict

all: $(PROGS)

# clock_gettime needs more than strict C17.
%: %.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE $< -o $@

run: all
	@echo "=== 01-conflict ==="; ./01-conflict; echo
	@echo "=== cache geometry ==="; lscpu -C 2>/dev/null || sysctl hw.cachelinesize hw.l1dcachesize hw.l2cachesize

clean:
	rm -f $(PROGS)
	rm -rf *.dSYM

.PHONY: all run clean
