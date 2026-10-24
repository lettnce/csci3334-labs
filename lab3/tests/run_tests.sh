#!/usr/bin/env bash
# Lab 3 test harness.
#
#   bash tests/run_tests.sh          every level   (or: make test)
#   bash tests/run_tests.sh 2        just level 2  (or: make test L=2)
#
# Each check runs ./mdriver on one trace. The driver checks every request
# (alignment, overlap, data kept) and some checks also need a utilization or
# a time limit. Levels are scored separately: if level 1 is green and level 3
# is red, you have 60 points plus whatever levels 2 and 4 earned.
#
# To see what happened on one trace, run the driver on it yourself:
#   ./mdriver -v traces/t1-lecture.tr        every request and its address
#   ./mdriver -v -d traces/t1-lecture.tr     ... and the heap after each one
#
# Grading uses this same script on your mm.c, plus traces you have not seen.
#
# Written for bash 3.2 too (the one macOS ships), so no associative arrays.

set -u

MDRIVER="${MDRIVER:-./mdriver}"
MMC="${MMC:-mm.c}"
CC="${CC:-gcc}"
ONLY="${1:-all}"
T=traces

[[ -x "$MDRIVER" ]] || { echo "no $MDRIVER: run 'make' first" >&2; exit 1; }

#            L1 L2 L3 L4 L5 bonus
weight=(0    60 15 10 10  5 10)
lp=(0 0 0 0 0 0 0); lt=(0 0 0 0 0 0 0)    # passed / total checks, by level
fails=0

pass() { printf '  \033[32mPASS\033[0m  %s  %s\n' "$1" "$2"; }
flunk() { printf '  \033[31mFAIL\033[0m  %s  %s\n' "$1" "$2"; fails=$((fails+1)); }
label() { [[ $1 -eq 6 ]] && echo "B " || echo "L$1"; }

# check <level> <description> <mdriver arguments...>
check() {
    local level="$1" desc="$2"; shift 2
    lt[$level]=$(( ${lt[$level]} + 1 ))
    [[ "$ONLY" != "all" && "$ONLY" != "$level" ]] && return
    local out
    out="$("$MDRIVER" "$@" 2>/dev/null)"
    if [[ $? -eq 0 ]]; then
        lp[$level]=$(( ${lp[$level]} + 1 ))
        pass "$(label "$level")" "$desc$(summary "$out")"
    else
        flunk "$(label "$level")" "$desc"
        echo "$out" | sed -e 's/^FAIL  //' -e 's/^/        /'
    fi
}

# The numbers worth seeing on a PASS line, from the driver's "ok:" line.
summary() {
    local line
    line="$(echo "$1" | grep '^ok' | tail -1)"
    case "$line" in
        *limit*)  echo "   ($(echo "$line" | sed -E 's/.*utilization ([0-9.]+%).*, ([0-9.]+ s) .*/util \1, \2/'))" ;;
        *needs*)  echo "   ($(echo "$line" | sed -E 's/.*utilization ([0-9.]+%).*/util \1/'))" ;;
        *) ;;
    esac
}

# Level 5: mm.c compiles with no warnings. Counts only once level 1 passes,
# so the untouched starter code does not score it. The bonus has the same rule.
check_warnings() {
    lt[5]=$(( ${lt[5]} + 1 ))
    [[ "$ONLY" != "all" && "$ONLY" != "5" ]] && return
    local w n
    w="$("$CC" -std=c17 -Wall -Wextra -O2 -fsyntax-only "$MMC" 2>&1)"
    n=$(echo "$w" | grep -c 'warning:')
    if [[ $n -ne 0 ]]; then
        flunk L5 "mm.c compiles with no warnings ($n warnings)"
        echo "$w" | grep 'warning:' | sed 's/^/        /'
    elif [[ ${lp[1]} -lt ${lt[1]} && "$ONLY" == "all" ]]; then
        flunk L5 "mm.c compiles with no warnings"
        echo "        no warnings, but this point counts only once every level 1 check passes"
    elif [[ "$ONLY" != "all" ]]; then
        lp[5]=$(( ${lp[5]} + 1 ))
        pass L5 "mm.c compiles with no warnings (counts once level 1 passes)"
    else
        lp[5]=$(( ${lp[5]} + 1 ))
        pass L5 "mm.c compiles with no warnings"
    fi
}

# Level 5: mm_check returns 0 after every request on several traces.
check_quiet() {
    lt[5]=$(( ${lt[5]} + 1 ))
    [[ "$ONLY" != "all" && "$ONLY" != "5" ]] && return
    local t out
    for t in t1-lecture t3-reuse t6-coalesce t7-mixed t8-realloc t9-calloc; do
        out="$("$MDRIVER" -c "$T/$t.tr" 2>&1)"
        if [[ $? -ne 0 ]]; then
            flunk L5 "mm_check returns 0 on a correct heap (-c on six traces)"
            echo "$out" | sed -e 's/^FAIL  //' -e 's/^/        /'
            return
        fi
    done
    lp[5]=$(( ${lp[5]} + 1 ))
    pass L5 "mm_check returns 0 on a correct heap (-c on six traces)"
}

echo
echo "Level 1: implicit free list, first fit (60 pts)"
check 1 "t1-lecture.tr  the Lecture 11 example"      "$T/t1-lecture.tr"
check 1 "t2-sizes.tr    block sizes and alignment"   "$T/t2-sizes.tr"
check 1 "t3-reuse.tr    free blocks are reused"      "$T/t3-reuse.tr"
check 1 "t4-churn.tr    random, small sizes"         -u 40 "$T/t4-churn.tr"

echo
echo "Level 2: splitting and coalescing (15 pts)"
check 2 "t5-split.tr    split a big free block"      "$T/t5-split.tr"
check 2 "t6-coalesce.tr merge with next and previous" "$T/t6-coalesce.tr"
check 2 "t7-mixed.tr    random, 8 bytes to 4 KB"     -u 55 "$T/t7-mixed.tr"

echo
echo "Level 3: realloc and calloc (10 pts)"
check 3 "t8-realloc.tr  realloc keeps the data"      -u 30 "$T/t8-realloc.tr"
check 3 "t9-calloc.tr   calloc zeroes, and overflow" "$T/t9-calloc.tr"

echo
echo "Level 4: an explicit free list (10 pts)"
check 4 "t10-many.tr    30,000 blocks, fast"         -u 65 -t 0.2 "$T/t10-many.tr"
check 4 "t11-waves.tr   waves of short-lived blocks" -u 65 -t 0.2 "$T/t11-waves.tr"

echo
echo "Level 5: heap checker and a clean build (5 pts)"
check_warnings
check_quiet
check 5 "mm_check catches a footer that disagrees with its header"   -C 1
check 5 "mm_check catches a size that is not a multiple of 16"       -C 2
check 5 "mm_check catches two free blocks side by side"              -C 3

echo
echo "Bonus: utilization (+10)"
check 6 "b1-realloc.tr  realloc grows in place"      -u 80 "$T/b1-realloc.tr"
check 6 "b2-fit.tr      a better fit than first fit" -u 80 "$T/b2-fit.tr"

echo
echo "-------------------------------------------------"
pts=0; bonus=0
for l in 1 2 3 4 5 6; do
    [[ "$ONLY" != "all" && "$ONLY" != "$l" ]] && continue
    t=${lt[$l]}; p=${lp[$l]}; w=${weight[$l]}
    earned=$(( w * p / t ))
    if [[ $l -eq 6 ]]; then
        if [[ ${lp[1]} -lt ${lt[1]} && $earned -gt 0 ]]; then
            earned=0
            note="   (counts once every level 1 check passes)"
        else
            note=""
        fi
        bonus=$earned
        printf 'Bonus:   %d/%d checks   +%d/%d%s\n' "$p" "$t" "$earned" "$w" "$note"
    else
        pts=$((pts + earned))
        printf 'Level %d: %d/%d checks   %2d/%d pts\n' "$l" "$p" "$t" "$earned" "$w"
    fi
done
if [[ "$ONLY" == "all" ]]; then
    printf 'Score: %d/100, bonus +%d\n' "$pts" "$bonus"
fi
echo "-------------------------------------------------"

[[ $fails -eq 0 ]]
