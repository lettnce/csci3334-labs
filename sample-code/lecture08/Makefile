# Lecture 8 examples.
#
#   make          build and they run on THIS machine (whatever it is)
#   make run      run them all in order
#   make asm      x86-64 assembly (AT&T, as CS:APP)  ->  NN-name.s
#   make dis      x86-64 disassembly with bytes  ->  NN-name.dis
#   make clean
#
# The course teaches x86-64. If you are on an Apple Silicon Mac, `make` still
# builds and runs native arm64 binaries, and `make asm` / `make dis` still
# produce real x86-64 — clang cross-compiles it. You just cannot execute the
# x86-64 binaries without Rosetta.

CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -Werror
PROGS   = 01-overflow 02-strncpy 03-ub 04-aliasing 05-aslr

# What we read in class. -O1 keeps it short; -fomit-frame-pointer drops the
# pushq %rbp / movq %rsp, %rbp preamble that has nothing to do with the point.
X86     = -O1 -fomit-frame-pointer -fno-asynchronous-unwind-tables
ARCH    = $(shell uname -m)
ifeq ($(ARCH),arm64)
X86FLAGS = -arch x86_64
else ifeq ($(ARCH),x86_64)
X86FLAGS =
else
X86FLAGS = -arch x86_64
endif

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

run: all
	@for p in $(PROGS); do \
	    echo "=== $$p ==="; ./$$p; echo; \
	done

# x86-64 assembly in AT&T syntax — the same syntax CS:APP and gdb use.
asm: $(PROGS:=.s)

%.s: %.c
	$(CC) $(X86FLAGS) $(X86) -S $< -o $@

# The same code with the encoded bytes next to it.
dis: $(PROGS:=.dis)

%.dis: %.c
	$(CC) $(X86FLAGS) $(X86) -c $< -o $*.o
	objdump -d $*.o > $@
	@rm -f $*.o

# Undefined behaviour changes answer with -O. Build 03-ub both ways and run
# both, so the difference is visible side by side.
ub: 03-ub.c
	@$(CC) $(CFLAGS) -O0 $< -o 03-ub-O0
	@$(CC) $(CFLAGS) -O1 $< -o 03-ub-O1
	@echo "=== -O0 ==="; ./03-ub-O0
	@echo; echo "=== -O1 ==="; ./03-ub-O1

# AddressSanitizer build of the overflow demo.
asan: 01-overflow.c
	$(CC) $(CFLAGS) -fsanitize=address -g $< -o 01-overflow-asan
	@echo "run:  ./01-overflow-asan AAAAAAAAAAAAAAAAAAAAAAAA"

clean:
	rm -f $(PROGS) *.s *.dis *.o 03-ub-O0 03-ub-O1 01-overflow-asan
	rm -rf *.dSYM

.PHONY: all run asm dis ub asan clean
