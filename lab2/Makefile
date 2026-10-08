# Lab 2: Assembly — Reverse Engineering & the Bomb
#
#   make                build ./btest
#   make test           check the warmup and all five mystery functions
#   make test P=0       check only the warmup, mystery0
#   make test P=3       check only mystery3
#   make disas P=3      disassemble mystery3.o
#
# Instructor targets (need _instructor/, which is not in the handout):
#   make verify         build btest against the answer key; expects 60/60 +10
#   make bomb           build the class bomb + answer key (change SEED yearly)
#   make mysteries      rebuild mystery0.o .. mystery5.o from source
#   make handout        build a student zip with the answers removed

CC      = gcc
CFLAGS  = -std=c17 -Wall -Wextra -g -O0
OBJS    = mystery0.o mystery1.o mystery2.o mystery3.o mystery4.o mystery5.o

.PHONY: all test disas verify bomb mysteries handout clean

all: btest

btest: tests/btest.c solutions.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test: btest
	@./btest $(P)

disas:
	@objdump -d --no-show-raw-insn mystery$(or $(P),1).o

# --- instructor side ------------------------------------------------------
#
# -O1 on purpose. At -O0 the assembly is a transcription of the C and there is
# nothing to work out; at -O2 the compiler turns mystery1's loop into a closed
# form and the exercise evaporates. Debug info is stripped so the answer is not
# sitting in .debug_info, but the symbol table stays so the object still links.
# -fcf-protection=none keeps Ubuntu's gcc from opening every function with an
# endbr64 that the lectures never mention.

mysteries:
	@for n in 0 1 2 3 4 5; do \
	    $(CC) -std=c17 -O1 -fcf-protection=none -DMYSTERY=$$n -c -o mystery$$n.o \
	          _instructor/mystery_src.c; \
	    objcopy --strip-debug mystery$$n.o; \
	done
	@echo "rebuilt: $(OBJS)"
	@for n in 0 1 2 3 4 5; do \
	    c=$$(nm --defined-only mystery$$n.o | grep -c ' T mystery'); \
	    [ "$$c" = "1" ] || { echo "ERROR: mystery$$n.o defines $$c functions"; exit 1; }; \
	done
	@echo "verified: one function per object"

# The course bomb — one for the whole class.
#
# Deterministic in SEED: the same seed always rebuilds a byte-identical bomb,
# so it does not need to be archived. CHANGE THE SEED EACH SEMESTER — reusing
# one reuses its answers, which will be circulating by then.
#
# ./bomb is committed and shipped in the handout. The answer key lands in
# _instructor/ and must never leave it.
SEED = csci3334-fall2026

bomb:
	@python3 _instructor/bomb/gen_bomb.py \
	    --owner "CSCI 3334" --seed "$(SEED)" --outdir _instructor/bomb-out
	@cp _instructor/bomb-out/bomb ./bomb
	@cp _instructor/bomb-out/solution.txt _instructor/bomb-solution.txt
	@rm -rf _instructor/bomb-out
	@echo
	@echo "./bomb                              <- hand this out (and commit it)"
	@echo "_instructor/bomb-solution.txt       <- the answers, INSTRUCTOR ONLY"

# End-to-end check of Part 1: build btest against the ANSWER KEY instead of the
# student stub and confirm it scores 60/60 +10 bonus. If this ever fails, the shipped
# mystery*.o and _instructor/mystery_src.c have drifted apart.
#
# Linux x86-64 only — the objects are ELF and will not link anywhere else.
verify:
	@case "$$(uname -s)/$$(uname -m)" in \
	  Linux/x86_64|Linux/amd64) ;; \
	  *) echo "make verify needs Linux x86-64 (this is $$(uname -s)/$$(uname -m))."; \
	     echo "The mystery*.o files are ELF x86-64 and will not link here."; \
	     exit 1 ;; \
	esac
	@$(CC) $(CFLAGS) -o btest-verify tests/btest.c _instructor/solutions-reference.c $(OBJS)
	@./btest-verify; rc=$$?; rm -f btest-verify; \
	  if [ $$rc -eq 0 ]; then echo "verify: OK — answer key matches the shipped objects"; \
	  else echo "verify: FAILED — objects and answer key disagree"; exit 1; fi

handout:
	@rm -rf /tmp/lab2-handout && mkdir -p /tmp/lab2-handout/lab2
	@cp -r Makefile solutions.c tests $(OBJS) /tmp/lab2-handout/lab2/
	@cp bomb /tmp/lab2-handout/lab2/ 2>/dev/null || { \
	    echo "ERROR: ./bomb missing — run 'make bomb' on Linux x86-64 first"; exit 1; }
	@cd /tmp/lab2-handout && zip -qr lab2.zip lab2
	@echo "handout at /tmp/lab2-handout/lab2.zip (Part 1 + the class bomb); upload it to Brightspace"
	@unzip -l /tmp/lab2-handout/lab2.zip | grep -q _instructor \
	    && echo "ERROR: answers leaked into the handout" && exit 1 || true

clean:
	rm -f btest
