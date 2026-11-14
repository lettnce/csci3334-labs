# Lecture 16 examples.
#
#   make          build them for THIS machine
#   make run      run them all in order (about 10 seconds)
#   make clean
#
# Everything builds and runs on Linux and macOS. The numbers depend on the
# machine; the slides show a Linux laptop with an AMD Ryzen 7 8845HS.
# perf, for the miss counts, is Linux only:
#   perf stat -e L1-dcache-load-misses ./02-matmul kij

CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -O1
PROGS   = 01-ways 02-matmul 03-falseshare

all: $(PROGS)

# clock_gettime and aligned_alloc need more than strict C17; threads need -pthread.
%: %.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE -pthread $< -o $@

run: all
	@echo "=== 01-ways ==="; ./01-ways; echo
	@echo "=== 02-matmul ==="; ./02-matmul; echo
	@echo "=== 03-falseshare ==="; ./03-falseshare; echo

clean:
	rm -f $(PROGS)
	rm -rf *.dSYM

.PHONY: all run clean
