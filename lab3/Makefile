# Lab 3: Memory Allocator
#
#   make              build ./mdriver from mm.c, mdriver.c and memlib.c
#   make test         run every level
#   make test L=1     run just level 1
#   make leaks        memory errors (valgrind, Linux/WSL) or leaks (macOS)
#
# To watch one trace:  ./mdriver -v -d traces/t1-lecture.tr
#
# Plain C: this lab builds and runs on Linux, WSL and macOS alike.
#
# Instructor targets (need _instructor/, which is not in the handout):
#   make traces       regenerate traces/ and _instructor/hidden/
#   make verify       answer key 100/100 +10 and every hidden check; stub scores 0
#   make levels       score a student who stops at each level (calibration)
#   make bugs         score the answer key with typical bugs planted in it
#   make grade F=path/to/mm.c   grade one submission
#   make handout      student zip, checked for leaked instructor files

CC     = gcc
CFLAGS = -std=c17 -Wall -Wextra -O2 -g
SRCS   = mdriver.c memlib.c
HDRS   = mm.h memlib.h

.PHONY: all test leaks traces verify levels bugs grade handout clean

all: mdriver

mdriver: $(SRCS) mm.c $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) mm.c

test: mdriver
	@bash tests/run_tests.sh $(if $(L),$(L),all)

leaks: mdriver
	@if command -v valgrind >/dev/null; then \
	    valgrind -q --error-exitcode=9 --leak-check=full \
	        ./mdriver traces/t7-mixed.tr >/dev/null; \
	    [ $$? -eq 9 ] && echo "valgrind found errors: run it without -q to see them" \
	                  || echo "valgrind: no memory errors"; \
	elif command -v leaks >/dev/null; then \
	    leaks -quiet --atExit -- ./mdriver traces/t7-mixed.tr >/dev/null \
	        && echo "no leaks" || echo "leaks found: run 'leaks --atExit -- ./mdriver traces/t7-mixed.tr' to see them"; \
	else \
	    echo "install valgrind (sudo apt install valgrind)"; \
	fi

# ---- instructor only ------------------------------------------------------

KEY = _instructor/mdriver-solution

$(KEY): $(SRCS) $(HDRS) _instructor/mm-solution.c
	$(CC) $(CFLAGS) -Werror -I. -o $@ $(SRCS) _instructor/mm-solution.c

traces:
	@python3 _instructor/make_traces.py

verify: $(KEY) mdriver
	@echo "== answer key (must be 100/100, bonus +10)"
	@MDRIVER=$(KEY) MMC=_instructor/mm-solution.c bash tests/run_tests.sh | tail -3
	@MDRIVER=$(KEY) MMC=_instructor/mm-solution.c bash tests/run_tests.sh | grep -q 'Score: 100/100, bonus +10' \
	    || { echo "ERROR: the answer key does not score 100 +10"; exit 1; }
	@echo "== answer key, hidden checks"
	@bash _instructor/grade.sh _instructor/mm-solution.c | grep -E '^   (hidden:|[0-9]+ warning|rules:|memory:)'
	@echo "== starter mm.c (must score 0)"
	@bash tests/run_tests.sh | grep -E '^Score'
	@bash tests/run_tests.sh | grep -q 'Score: 0/100, bonus +0' \
	    || { echo "ERROR: the starter code scores points"; exit 1; }

levels:
	@bash _instructor/score_variants.sh levels

bugs:
	@bash _instructor/score_variants.sh bugs

grade:
	@bash _instructor/grade.sh $(F)

HANDOUT = /tmp/lab3-handout
handout:
	@rm -rf $(HANDOUT) && mkdir -p $(HANDOUT)/lab3
	@cp -r Makefile mm.c mm.h mdriver.c memlib.c memlib.h tests traces $(HANDOUT)/lab3/
	@cp .gitignore $(HANDOUT)/lab3/
	@cp ../../pdf/labs/lab3.pdf $(HANDOUT)/lab3/ 2>/dev/null \
	    || echo "note: pdf/labs/lab3.pdf not found, handout has no PDF"
	@cd $(HANDOUT) && zip -qr lab3.zip lab3
	@unzip -l $(HANDOUT)/lab3.zip | grep -qE '_instructor|solution|hidden|levels|bugs' \
	    && { echo "ERROR: instructor files leaked into the handout"; exit 1; } || true
	@echo "handout at $(HANDOUT)/lab3.zip; upload it to Brightspace"

clean:
	rm -rf mdriver mdriver.dSYM
