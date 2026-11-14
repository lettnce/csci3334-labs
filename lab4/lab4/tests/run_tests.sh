#!/usr/bin/env bash
# Lab 4 test harness.
#
#   ./tests/run_tests.sh          every level
#   ./tests/run_tests.sh 1        just level 1
#
# Your ./csim is run on each trace and geometry below, and its output is
# compared with the reference simulator's output for the same run, saved in
# tests/expected/. A stub scores 0.
#
# Levels are checked in order and each is independently worth points, so if
# level 1 is green and level 3 is red, that is 60 of 60 plus whatever level 2
# earned — not a failure.
#
# Instructor use (needs the reference simulator, which is not in the handout):
#   REF=./refsim ./tests/run_tests.sh          compare with the reference live
#   REF=./refsim RECORD=1 ./tests/run_tests.sh rewrite tests/expected/
#
# Written for bash 3.2 too (the one macOS ships), so no associative arrays.

set -u

CSIM="${CSIM:-./csim}"
REF="${REF:-}"
RECORD="${RECORD:-}"
ONLY="${1:-all}"
EXP="tests/expected"

if [[ -n "$RECORD" ]]; then
    [[ -x "$REF" ]] || { echo "RECORD needs REF=<reference simulator>" >&2; exit 1; }
    mkdir -p "$EXP"
else
    [[ -x "$CSIM" ]] || { echo "no $CSIM — run 'make' first" >&2; exit 1; }
fi

pass=0; fail=0
lp=(0 0 0 0 0); lt=(0 0 0 0 0)    # passed / total checks, indexed by level

ok() { printf '  \033[32mPASS\033[0m  L%s  %s\n' "$1" "$2"
       pass=$((pass+1)); lp[$1]=$(( ${lp[$1]} + 1 )); }
no() { printf '  \033[31mFAIL\033[0m  L%s  %s\n' "$1" "$2"; fail=$((fail+1)); }

# check <level> <flags...> -t <trace>
check() {
    local level="$1"; shift
    lt[$level]=$(( ${lt[$level]} + 1 ))
    [[ "$ONLY" != "all" && "$ONLY" != "$level" ]] && return

    local desc="$*"
    # "-v -s 7 -E 1 -b 6 -t traces/t1-basic.tr" -> v_s7_E1_b6_t1-basic.out
    local file="$EXP/$(echo "$*" | sed -e 's#traces/##' -e 's/ -t /_/' \
        -e 's/\.tr$//' -e 's/^-//' -e 's/ -/_/g' -e 's/ //g').out"

    if [[ -n "$RECORD" ]]; then
        "$REF" "$@" > "$file"
        echo "  wrote $file"
        return
    fi

    local got want
    got="$("$CSIM" "$@" 2>/dev/null)"
    if [[ -n "$REF" ]]; then
        want="$("$REF" "$@" 2>/dev/null)"
    else
        want="$(cat "$file")"
    fi

    if [[ "$got" == "$want" ]]; then
        ok "$level" "$desc"
    else
        no "$level" "$desc"
        # Show the summary line rather than a whole verbose diff.
        printf '        expected: %s\n' "$(echo "$want" | tail -1)"
        printf '        actual:   %s\n' "$(echo "$got"  | tail -1)"
        if [[ "$*" == -v* ]]; then
            printf '        to see every line: diff <(%s) %s\n' "$CSIM $*" "$file"
        fi
    fi
}

echo
echo "Level 1 — direct-mapped (60 pts)"
check 1 -s 7 -E 1 -b 6 -t traces/t1-basic.tr       # the Lecture 15 slide
check 1 -s 6 -E 1 -b 6 -t traces/t2-conflict.tr    # a[i] + b[i], 4 KB cache
check 1 -s 7 -E 1 -b 6 -t traces/t2-conflict.tr    # same loop, 8 KB cache
check 1 -s 1 -E 1 -b 1 -t traces/t6-mixed.tr
check 1 -s 8 -E 1 -b 6 -t traces/t6-mixed.tr
check 1 -s 4 -E 1 -b 4 -t traces/t7-wide.tr        # 64-bit addresses

echo
echo "Level 2 — set-associative, no evictions (15 pts)"
check 2 -s 3 -E 4 -b 5 -t traces/t3-assoc-nofill.tr
check 2 -s 3 -E 8 -b 5 -t traces/t3-assoc-nofill.tr
check 2 -s 0 -E 32 -b 5 -t traces/t3-assoc-nofill.tr   # fully associative
check 2 -s 7 -E 2 -b 6 -t traces/t1-basic.tr

echo
echo "Level 3 — LRU eviction (10 pts)"
check 3 -s 2 -E 3 -b 5 -t traces/t4-lru.tr
check 3 -s 4 -E 2 -b 4 -t traces/t6-mixed.tr
check 3 -s 5 -E 4 -b 5 -t traces/t6-mixed.tr
check 3 -s 2 -E 8 -b 3 -t traces/t6-mixed.tr
check 3 -s 3 -E 2 -b 4 -t traces/t7-wide.tr

echo
echo "Level 4 — verbose mode (5 pts)"
check 4 -v -s 4 -E 1 -b 4 -t traces/t5-verbose.tr
check 4 -v -s 3 -E 4 -b 5 -t traces/t3-assoc-nofill.tr
check 4 -v -s 7 -E 1 -b 6 -t traces/t1-basic.tr

[[ -n "$RECORD" ]] && exit 0

echo
echo "-------------------------------------------"
pts=0
for l in 1 2 3 4; do
    t=${lt[$l]}; p=${lp[$l]}
    [[ "$ONLY" != "all" && "$ONLY" != "$l" ]] && continue
    case $l in 1) w=60;; 2) w=15;; 3) w=10;; 4) w=5;; esac
    earned=$(( w * p / t ))
    pts=$((pts + earned))
    printf 'Level %s: %d/%-2d checks   %2d/%d pts\n' "$l" "$p" "$t" "$earned" "$w"
done
if [[ "$ONLY" == "all" ]]; then
    printf 'Score: %d/90   (level 5, code quality, is 10 more and is graded by hand)\n' "$pts"
fi
echo "-------------------------------------------"

[[ $fail -eq 0 ]]
