# Lecture 13 examples.
#
#   make          build them for THIS machine
#   make run      run them all in order
#   make clean
#
# Everything builds and runs on Linux and macOS. The stat -c commands on the
# slides are the Linux (GNU) form; macOS stat uses -f instead.

CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -O1
PROGS   = 01-myls 02-unlink 03-forkpos

all: $(PROGS)

# The POSIX calls need more than strict C17.
%: %.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE $< -o $@

run: all
	@echo "=== 01-myls ==="; ./01-myls; echo
	@echo "=== 02-unlink ==="; ./02-unlink; echo
	@echo "=== 03-forkpos ==="; ./03-forkpos; echo

clean:
	rm -f $(PROGS) data.txt temp.txt
	rm -rf *.dSYM

.PHONY: all run clean
