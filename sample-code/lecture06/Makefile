# Lecture 6 examples.
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
PROGS   = 01-first-look 02-lea 03-shapes 04-sizes

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

clean:
	rm -f $(PROGS) *.s *.dis *.o

.PHONY: all run asm dis clean
