# Lab 5: Simple Shell
#
#   make              build ./shell (and the test driver)
#   make test         run every level
#   make test L=1     run only Level 1
#   make test L=bonus run only the bonus
#   make leaks        leak check: valgrind on Linux/WSL, leaks on macOS
#
# Plain C and POSIX: this lab builds and runs on Linux, WSL and macOS alike.
#
# Instructor targets (need _instructor/, which is not in the handout):
#   make verify               answer key must score 90/90 + 5 bonus and pass every hidden check
#   make grade F=path/shell.c grade one submission
#   make bugs                 run the planted-bug versions of the key
#   make handout              student zip, checked for leaked instructor files

CC     = gcc
CFLAGS = -std=c17 -Wall -Wextra -g -O0

.PHONY: all test leaks verify grade bugs handout clean

all: shell tests/driver

shell: shell.c
	$(CC) $(CFLAGS) -o $@ $<

tests/driver: tests/driver.c
	$(CC) $(CFLAGS) -o $@ $<

test: shell tests/driver
	@bash tests/run_tests.sh $(if $(L),$(L),all)

# A short session that touches every level, run under a leak checker.
LEAKS_INPUT = printf 'pwd\ncd /\npwd\ncd\n/bin/echo hi > /dev/null\necho a | cat\nsleep 0.1 &\njobs\n'
leaks: shell
	@if command -v valgrind >/dev/null; then \
	    $(LEAKS_INPUT) | valgrind -q --leak-check=full --error-exitcode=1 ./shell >/dev/null \
	        && echo "no leaks"; \
	elif command -v leaks >/dev/null; then \
	    $(LEAKS_INPUT) | leaks -quiet --atExit -- ./shell > leaks-report.txt 2>/dev/null \
	        && echo "no leaks" \
	        || { grep -E 'leaks for|ROOT LEAK|shell +0x' leaks-report.txt; \
	             echo "leaks found: the full report is in leaks-report.txt"; }; \
	else \
	    echo "install valgrind (sudo apt install valgrind)"; \
	fi

# ---- instructor only ------------------------------------------------------

verify: tests/driver
	@$(CC) $(CFLAGS) -Werror -o _instructor/shell-solution _instructor/shell-solution.c
	@SHELL_BIN=_instructor/shell-solution bash tests/run_tests.sh | tail -8
	@bash _instructor/grade.sh _instructor/shell-solution.c | grep -E 'hidden:|warning|clean|LEAK|FORBIDDEN|^   none'

grade: tests/driver
	@bash _instructor/grade.sh $(F)

bugs: tests/driver
	@bash _instructor/plant-bugs.sh

HANDOUT = /tmp/lab5-handout
handout:
	@rm -rf $(HANDOUT) && mkdir -p $(HANDOUT)/lab5/tests
	@cp Makefile shell.c $(HANDOUT)/lab5/
	@cp tests/run_tests.sh tests/driver.c $(HANDOUT)/lab5/tests/
	@cp ../../pdf/labs/lab5.pdf $(HANDOUT)/lab5/ 2>/dev/null \
	    || echo "note: pdf/labs/lab5.pdf not found, handout has no PDF"
	@cd $(HANDOUT) && zip -qr lab5.zip lab5
	@unzip -l $(HANDOUT)/lab5.zip | grep -qE '_instructor|solution|hidden|bugs|grade' \
	    && { echo "ERROR: instructor files leaked into the handout"; exit 1; } || true
	@echo "handout at $(HANDOUT)/lab5.zip; upload it to Brightspace"

clean:
	rm -rf shell tests/driver leaks-report.txt *.dSYM tests/*.dSYM
