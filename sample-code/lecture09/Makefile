# Lecture 9 examples.
#
#   make          build them for THIS machine (whatever it is)
#   make run      run them all in order (03-divide may be killed; that is the point)
#   make linux    the two Linux x86-64 only programs: 01-privileged and 04-hello
#   make gpu      11-launches.cu, for the AI slide (needs an NVIDIA GPU and nvcc)
#   make clean
#
# Most of these run anywhere. The ones that are about Linux itself are not:
# 01-privileged and 04-hello contain x86-64 instructions, 04-hello uses Linux
# system call numbers, and strace and /proc exist only on Linux. On a Mac, run
# those parts on Linux, WSL2 or the lab machines. 10-context-switches runs on
# a Mac, but macOS counts context switches differently; use Linux numbers.

CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -O1
PROGS   = 02-page-faults 03-divide 05-errno 06-printf 07-buffering \
          08-syscall-cost 09-pid 10-context-switches

all: $(PROGS)

# 08 and 10 need clock_gettime / usleep, which strict -std=c17 hides on Linux.
%: %.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE $< -o $@

run: all
	@for p in $(PROGS); do \
	    echo "=== $$p ==="; if [ $$p = 07-buffering ]; then ./$$p | tail -1; else ./$$p; fi; echo; \
	done

# 04-hello has no C library and no startup code: it begins at _start and
# makes exactly two system calls. -static because there is nothing to link.
linux: 01-privileged.c 04-hello.s
	@if [ "$$(uname -s)-$$(uname -m)" != "Linux-x86_64" ]; then \
	    echo "These are Linux x86-64 programs; build them on a Linux x86-64 machine."; exit 1; fi
	gcc -O1 01-privileged.c -o 01-privileged
	gcc -nostdlib -static 04-hello.s -o 04-hello

# Not part of `all`: it needs an NVIDIA GPU. -arch=native builds for the GPU
# in this machine.
gpu: 11-launches.cu
	nvcc -O2 -arch=native $< -o 11-launches

clean:
	rm -f $(PROGS) 01-privileged 04-hello 11-launches *.o *.txt
	rm -rf *.dSYM

.PHONY: all run linux gpu clean
