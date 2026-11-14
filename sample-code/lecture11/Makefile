# Lecture 11 examples.
#
#   make          build them for THIS machine
#   make run      run them all
#   make clean
#
# sbrk works on Linux and macOS. strace (to see the brk system call) is Linux
# only.

CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -O1
# macOS marks sbrk deprecated; the warning is not the point here.
ifeq ($(shell uname -s),Darwin)
CFLAGS += -Wno-deprecated-declarations
endif
PROGS   = 01-sbrk 02-malloc-brk-mmap

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

run: all
	@for p in $(PROGS); do echo "=== $$p ==="; ./$$p; echo; done

clean:
	rm -f $(PROGS)
	rm -rf *.dSYM

.PHONY: all run clean
