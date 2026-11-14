# Lecture 14 examples.
#
#   make          build them for THIS machine
#   make run      run them all in order (10 to 20 seconds)
#   make clean
#
# Everything builds and runs on Linux and macOS. The numbers depend on the
# machine; the slides show a Linux laptop with an AMD Ryzen 7 8845HS.

CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -O1
PROGS   = 01-stride 02-rowcol 03-workingset

all: $(PROGS)

# clock_gettime and random need more than strict C17.
%: %.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE $< -o $@

run: all
	@echo "=== 01-stride ==="; ./01-stride; echo
	@echo "=== 02-rowcol ==="; ./02-rowcol; echo
	@echo "=== 03-workingset ==="; ./03-workingset; echo

clean:
	rm -f $(PROGS)
	rm -rf *.dSYM

.PHONY: all run clean
