# Lab 4: Cache Simulator
#
#   make              build ./csim
#   make test         run every level
#   make test L=1     run just level 1
#   make leaks        leak check (level 5): valgrind on Linux/WSL, leaks on macOS
#
# Plain C: this lab builds and runs on Linux, WSL and macOS alike. The tests
# compare against saved reference output in tests/expected/, so no reference
# binary is needed.
#
# Instructor targets (need _instructor/, which is not in the handout):
#   make traces       regenerate traces/, partb/ and _instructor/hidden/
#   make expected     regenerate tests/expected/ from the reference
#   make verify       answer key must score 100% on the harness and hidden checks
#   make grade F=path/to/csim.c   grade one submission
#   make handout      student zip, checked for leaked instructor files

CC     = gcc
CFLAGS = -std=c17 -Wall -Wextra -g -O0
TRACE  = traces/t6-mixed.tr

.PHONY: all test leaks traces expected verify grade handout clean

all: csim

csim: csim.c
	$(CC) $(CFLAGS) -o $@ $<

test: csim
	@bash tests/run_tests.sh $(if $(L),$(L),all)

leaks: csim
	@if command -v valgrind >/dev/null; then \
	    valgrind -q --leak-check=full --error-exitcode=1 \
	        ./csim -s 4 -E 2 -b 4 -t $(TRACE) >/dev/null && echo "no leaks"; \
	elif command -v leaks >/dev/null; then \
	    leaks -quiet --atExit -- ./csim -s 4 -E 2 -b 4 -t $(TRACE) >/dev/null \
	        && echo "no leaks" || echo "leaks found: run 'leaks --atExit -- ./csim ...' to see them"; \
	else \
	    echo "install valgrind (sudo apt install valgrind)"; \
	fi

# ---- instructor only ------------------------------------------------------

_instructor/refsim: _instructor/refsim.c
	$(CC) $(CFLAGS) -o $@ $<

refsim: _instructor/refsim

traces:
	@python3 _instructor/make_traces.py

expected: _instructor/refsim
	@rm -rf tests/expected
	@REF=_instructor/refsim RECORD=1 bash tests/run_tests.sh | tail -1
	@echo "tests/expected/ rewritten: $$(ls tests/expected | wc -l | tr -d ' ') files"

verify: _instructor/refsim
	@$(CC) $(CFLAGS) -Werror -o _instructor/csim-solution _instructor/csim-solution.c
	@CSIM=_instructor/csim-solution bash tests/run_tests.sh | tail -3
	@CSIM=_instructor/csim-solution REF=_instructor/refsim bash tests/run_tests.sh >/dev/null \
	    && echo "expected files agree with the live reference" \
	    || { echo "ERROR: tests/expected/ is stale — run 'make expected'"; exit 1; }
	@bash _instructor/grade.sh _instructor/csim-solution.c | grep -E 'hidden:|warning|clean|LEAK'

grade: _instructor/refsim
	@bash _instructor/grade.sh $(F)

HANDOUT = /tmp/lab4-handout
handout:
	@rm -rf $(HANDOUT) && mkdir -p $(HANDOUT)/lab4
	@cp -r Makefile csim.c tests traces partb $(HANDOUT)/lab4/
	@cp ../../pdf/labs/lab4.pdf $(HANDOUT)/lab4/ 2>/dev/null \
	    || echo "note: pdf/labs/lab4.pdf not found, handout has no PDF"
	@cd $(HANDOUT) && zip -qr lab4.zip lab4
	@unzip -l $(HANDOUT)/lab4.zip | grep -qE '_instructor|refsim|solution|hidden' \
	    && { echo "ERROR: instructor files leaked into the handout"; exit 1; } || true
	@echo "handout at $(HANDOUT)/lab4.zip; upload it to Brightspace"

clean:
	rm -f csim
