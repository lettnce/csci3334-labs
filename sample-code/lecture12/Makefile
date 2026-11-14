# Lecture 12 examples.
#
#   make          build them for THIS machine
#   make run      run them all in order
#   make clean
#
# Everything builds and runs on Linux and macOS. strace (to see the system
# calls) and /proc/self/fd are Linux only.

CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -O1
PROGS   = 01-mycat 02-short-read 03-buffering 04-stat

all: $(PROGS)

# clock_gettime and the POSIX calls need more than strict C17.
%: %.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE $< -o $@

run: all
	@echo "=== 01-mycat ==="; echo hello | ./01-mycat; echo
	@echo "=== 02-short-read ==="; (echo hello; sleep 1; echo world) | ./02-short-read; echo
	@echo "=== 03-buffering ==="; ./03-buffering; echo
	@echo "=== 04-stat ==="; ./04-stat 04-stat.c; echo

clean:
	rm -f $(PROGS) out.bin
	rm -rf *.dSYM

.PHONY: all run clean
