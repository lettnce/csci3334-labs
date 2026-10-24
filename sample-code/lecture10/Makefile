# Lecture 10 examples.
#
#   make          build them for THIS machine
#   make run      run them all in order
#   make clean
#
# Everything builds and runs on Linux and macOS. Page-fault counts (01, 05)
# are exact only on Linux, and /proc/self/maps (07) exists only on Linux.

CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -O1
PROGS   = 01-page-faults-twice 02-mmap-anon 03-mmap-file 04-mmap-shared \
          05-cow 06-protect 07-newlines

all: $(PROGS)

# mmap's MAP_ANONYMOUS, strsignal and getrusage need more than strict C17.
%: %.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE $< -o $@

run: all
	@for p in $(PROGS); do \
	    echo "=== $$p ==="; \
	    case $$p in 03-*|07-*) ./$$p $$p.c;; *) ./$$p;; esac; echo; \
	done

clean:
	rm -f $(PROGS)
	rm -rf *.dSYM

.PHONY: all run clean
