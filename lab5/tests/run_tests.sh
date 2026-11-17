#!/usr/bin/env bash
# Lab 5 test harness.
#
#   ./tests/run_tests.sh          run every level
#   ./tests/run_tests.sh 1        run only Level 1
#   ./tests/run_tests.sh bonus    run only the bonus (Ctrl-C and Ctrl-Z)
#
# Each check writes a few commands into a script, runs your ./shell on it
# through tests/driver (which also plays Ctrl-C and Ctrl-Z, and gives up on a
# shell that hangs), and compares what comes out with what a correct shell
# prints. An unimplemented skeleton scores 0. If you ever see a check pass
# that you did not earn, that is a bug in this harness: please tell me.
#
# Levels are scored separately, so if Level 1 is green and Level 3 is red you
# still have every Level 1 point.
#
# Written for bash 3.2 too (the one macOS ships), so no associative arrays.

set -u

KIT="$(cd "$(dirname "$0")/.." && pwd -P)"
SHELL_BIN="${SHELL_BIN:-$KIT/shell}"
SHELL_BIN="$(cd "$(dirname "$SHELL_BIN")" && pwd -P)/$(basename "$SHELL_BIN")"
DRIVER="$KIT/tests/driver"
ONLY="${1:-all}"
case "$ONLY" in b|B|bonus|Bonus) ONLY=6;; esac    # the bonus is level 6 inside

[[ -x "$SHELL_BIN" ]] || { echo "no shell at $SHELL_BIN: run 'make' first" >&2; exit 1; }
[[ -x "$DRIVER" ]]    || { echo "no $DRIVER: run 'make' first" >&2; exit 1; }

TMP="$(mktemp -d)"
TMP="$(cd "$TMP" && pwd -P)"     # resolve symlinks, so it matches what pwd prints
trap 'rm -rf "$TMP"' EXIT
HOME_P="$(cd "$HOME" && pwd -P)"

pass=0; fail=0
lp=(0 0 0 0 0 0 0); lt=(0 0 0 0 0 0 0)   # passed / total checks; index 6 = bonus

tag() { if [[ $1 -eq 6 ]]; then printf 'B '; else printf 'L%s' "$1"; fi; }
ok() { printf '  \033[32mPASS\033[0m  %s  %s\n' "$(tag "$1")" "$2"
       pass=$((pass+1)); lp[$1]=$(( ${lp[$1]} + 1 )); }
no() { printf '  \033[31mFAIL\033[0m  %s  %s\n' "$(tag "$1")" "$2"; fail=$((fail+1)); }

# Fresh files for every check, so one check cannot leave work for the next.
reset_dir() {
    rm -rf "$TMP/w"; mkdir "$TMP/w"
    printf 'hello\nworld\n'  > "$TMP/w/data.txt"
    printf 'from-input\n'    > "$TMP/w/input.txt"
    printf 'old contents, longer than the new ones\n' > "$TMP/w/out.txt"
    printf 'pear\napple\nfig\napple\npear\napple\nkiwi\npear\napple\n' > "$TMP/w/words.txt"
    printf 'sleep 0.5\necho slow\n'                > "$TMP/w/slow.sh"
    printf 'echo to-stdout\necho to-stderr >&2\n'  > "$TMP/w/both.sh"
    printf 'sleep 0.5\necho done > mark.txt\n'     > "$TMP/w/mark.sh"
}

# run <seconds> <script>: run the shell on the script inside $TMP/w.
# Leaves its stdout in $OUT, its stderr in $ERR and the driver's status in $RC.
OUT="$TMP/stdout"; ERR="$TMP/stderr"
run() {
    printf '%s\n' "$2" > "$TMP/script"
    ( cd "$TMP/w" && "$DRIVER" "$1" "$TMP/script" "$SHELL_BIN" ) > "$OUT" 2> "$ERR"
    RC=$?
}

# Explain a run that did not end normally.
why() {
    if [[ $RC -eq 124 ]]; then
        echo "        the shell did not finish in time and was killed. It hung, or"
        echo "        Ctrl-Z stopped the shell itself. Hangs come from a pipe end left"
        echo "        open or a waitpid for a child that will never exit."
    elif [[ $RC -gt 128 ]]; then
        echo "        the shell itself was killed by signal $((RC - 128))"
    fi
}

# Should this check run? It always counts toward the level's total.
want() {
    lt[$1]=$(( ${lt[$1]} + 1 ))
    [[ "$ONLY" == "all" || "$ONLY" == "$1" ]] || return 1
    reset_dir
}

# show <what> <expected> <actual>: print both, one "| " line per line.
show() {
    printf '        %s expected:\n' "$1"; block "$2"
    printf '        %s actual:\n'   "$1"; block "$3"
}
block() {
    if [[ -z "$1" ]]; then echo "            (nothing)"
    else printf '%s\n' "$1" | sed 's/^/            | /'; fi
}

# check <level> <name> <script> <expected stdout> [seconds]
check() {
    want "$1" || return 0
    run "${5:-3}" "$3"
    local got; got="$(cat "$OUT")"
    if [[ "$got" == "$4" ]]; then ok "$1" "$2"; else
        no "$1" "$2"; show "stdout" "$4" "$got"; why
    fi
}

# check_sh <level> <name> <script>: the expected output is whatever /bin/sh
# prints for the same commands on this machine. Used for pipelines whose
# output differs between macOS and Linux (wc and uniq pad differently).
check_sh() {
    want "$1" || return 0
    local expected
    expected="$(cd "$TMP/w" && printf '%s\n' "$3" | /bin/sh 2>/dev/null)"
    reset_dir
    run 3 "$3"
    local got; got="$(cat "$OUT")"
    if [[ "$got" == "$expected" ]]; then ok "$1" "$2"; else
        no "$1" "$2"; show "stdout" "$expected" "$got"; why
    fi
}

# check_err <level> <name> <script> <expected stdout> <text stderr must contain>
check_err() {
    want "$1" || return 0
    run 3 "$3"
    local got; got="$(cat "$OUT")"
    if [[ "$got" != "$4" ]]; then
        no "$1" "$2"; show "stdout" "$4" "$got"; why
    elif ! grep -q -- "$5" "$ERR"; then
        no "$1" "$2"; echo "        stderr should mention '$5' but was: $(head -c 200 "$ERR")"
    else
        ok "$1" "$2"
    fi
}

# check_file <level> <name> <script> <expected stdout> <file> <expected contents>
check_file() {
    want "$1" || return 0
    run 3 "$3"
    local got; got="$(cat "$OUT")"
    if [[ ! -f "$TMP/w/$5" ]]; then
        no "$1" "$2"; echo "        $5 was never created"; why; return
    fi
    local inside; inside="$(cat "$TMP/w/$5")"
    if [[ "$inside" != "$6" ]]; then
        no "$1" "$2"; show "$5" "$6" "$inside"; why
    elif [[ "$got" != "$4" ]]; then
        no "$1" "$2"; show "stdout" "$4" "$got"; why
    else
        ok "$1" "$2"
    fi
}

# Job-control output: lines that start with "[" are the shell's own job
# messages ("[1] 4242", "[1] Done sleep 1"). These compare everything else.
plain() { grep -v '^\[' "$OUT"; }

# check_jobs <level> <name> <script> <expected non-[ lines> <seconds> <regex>...
#   Every regex must match some line of stdout.
check_jobs() {
    want "$1" || return 0
    local level="$1" name="$2" script="$3" expected="$4" secs="$5"
    shift 5
    run "$secs" "$script"
    local got; got="$(plain)"
    if [[ "$got" != "$expected" ]]; then
        no "$level" "$name"; show "stdout" "$expected" "$got"
        echo "        (lines starting with [ are left out of this comparison)"
        why; return
    fi
    local re
    for re in "$@"; do
        if [[ "$re" == "zombies=0" ]]; then
            grep -q 'driver: zombies=0' "$ERR" && continue
            no "$level" "$name"
            echo "        a finished background job was never reaped:"
            echo "        $(grep 'driver: zombies' "$ERR")"
            return
        fi
        if ! grep -Eq -- "$re" "$OUT"; then
            no "$level" "$name"
            echo "        no line of stdout matches: $re"
            echo "        stdout was: $(tr '\n' '|' < "$OUT")"
            return
        fi
    done
    ok "$level" "$name"
}

NL=$'\n'

# ---------------------------------------------------------------- Level 1
if [[ "$ONLY" == all || "$ONLY" == 1 ]]; then
    echo; echo "Level 1 — Running commands and built-ins (60 pts)"
fi

check     1 "run a program"              '/bin/echo hi'                    'hi'
check     1 "program with arguments"     '/bin/echo a b c'                 'a b c'
check     1 "PATH search (no slash)"     'echo found-on-path'              'found-on-path'
check     1 "several commands in order"  "/bin/echo one$NL/bin/echo two"   "one${NL}two"
check     1 "blank lines are ignored"    "$NL   $NL/bin/echo after"        'after'
check     1 "waits for each command"     "sh slow.sh$NL/bin/echo fast"     "slow${NL}fast"
check     1 "pwd built-in"               'pwd'                             "$TMP/w"
check     1 "cd then pwd"                "cd /${NL}pwd"                    '/'
check     1 "cd changes the shell's directory" "cd /${NL}/bin/pwd"         '/'
check     1 "cd with no argument goes HOME" "cd${NL}pwd"                   "$HOME_P"
check_err 1 "cd to a missing directory"  "cd /no/such/dir-xyz${NL}pwd"     "$TMP/w" 'dir-xyz'
check     1 "exit stops the shell"       "/bin/echo before${NL}exit${NL}/bin/echo after" 'before'
check_err 1 "unknown command"            "nosuchcommand-xyz$NL/bin/echo alive" 'alive' 'nosuchcommand-xyz'
check     1 "built-in output stays in order" \
                                         "pwd${NL}nosuchcommand-xyz${NL}/bin/echo end" "$TMP/w${NL}end"

# ---------------------------------------------------------------- Level 2
if [[ "$ONLY" == all || "$ONLY" == 2 ]]; then
    echo; echo "Level 2 — I/O redirection (10 pts)"
fi

check_file 2 "> replaces the file"       '/bin/echo new > out.txt'   '' 'out.txt' 'new'
check_file 2 ">> appends"                "/bin/echo a > ap.txt$NL/bin/echo b >> ap.txt" '' 'ap.txt' "a${NL}b"
check      2 "< reads from a file"       'tr a-z A-Z < input.txt'    'FROM-INPUT'
check_file 2 "2> sends stderr to a file" 'sh both.sh 2> err.txt'     'to-stdout' 'err.txt' 'to-stderr'
check_file 2 "> file 2>&1 sends both"    'sh both.sh > all.txt 2>&1' '' 'all.txt' "to-stdout${NL}to-stderr"
check_file 2 "2>&1 > file (order matters)" 'sh both.sh 2>&1 > o2.txt' 'to-stderr' 'o2.txt' 'to-stdout'
check_err  2 "< a missing file is an error" \
                                         "tr a-z A-Z < nosuch.txt$NL%SLEEP 0.3$NL/bin/echo alive" 'alive' 'nosuch.txt'

# ---------------------------------------------------------------- Level 3
if [[ "$ONLY" == all || "$ONLY" == 3 ]]; then
    echo; echo "Level 3 — Pipes (10 pts)"
fi

check_sh   3 "two commands"              'cat data.txt | wc -l'
check      3 "three commands"            'cat data.txt | sort -r | head -1'  'world'
check_sh   3 "five commands" 'cat words.txt | sort | uniq -c | sort -rn | head -3'
check_file 3 "pipe plus redirection"     'cat < data.txt | sort -r > top.txt' '' 'top.txt' "world${NL}hello"
check      3 "waits for every command"   "sh mark.sh | /bin/echo x${NL}cat mark.txt"  "x${NL}done"
check      3 "commands after a pipeline" "cat data.txt | sort${NL}/bin/echo next"     "hello${NL}world${NL}next"

# ---------------------------------------------------------------- Level 4
if [[ "$ONLY" == all || "$ONLY" == 4 ]]; then
    echo; echo "Level 4 — Background jobs (5 pts)"
fi

check_jobs 4 "& does not wait" \
    "sh slow.sh &$NL/bin/echo fast$NL%SLEEP 1" "fast${NL}slow" 3 \
    '^\[1\] [0-9]+$'
check_jobs 4 "jobs lists background jobs" \
    "sleep 1 &${NL}sleep 2 &${NL}jobs" "" 3 \
    '^\[1\].*Running.*sleep 1' '^\[2\].*Running.*sleep 2'
check_jobs 4 "fg %1 waits for the job" \
    "sh slow.sh &${NL}fg %1${NL}/bin/echo after" "slow${NL}after" 3 \
    '^\[1\] [0-9]+$'
check_jobs 4 "finished jobs are reaped" \
    "sleep 0.2 &$NL%SLEEP 0.6$NL/bin/echo tick$NL%SLEEP 0.3$NL%ZOMBIES${NL}jobs" "tick" 3 \
    '^\[1\] [0-9]+$' 'zombies=0'

# ---------------------------------------------------------------- Level 5
if [[ "$ONLY" == all || "$ONLY" == 5 ]]; then
    echo; echo "Level 5 — && and || (5 pts)"
fi

check      5 "&& runs the next command on success" \
                                         '/bin/echo a && /bin/echo b'      "a${NL}b"
check      5 "&& skips the next command on failure" \
                                         "ls nosuch-xyz && /bin/echo no${NL}/bin/echo end" 'end'
check      5 "|| runs the next command on failure" \
                                         'false || /bin/echo yes'          'yes'
check      5 "|| skips the next command on success" \
                                         '/bin/echo yes || /bin/echo no'   'yes'
check      5 "a skipped command keeps the status" \
    "false && /bin/echo no || /bin/echo yes${NL}true || /bin/echo no && /bin/echo yes2" "yes${NL}yes2"
check      5 "an unknown command fails (status 127)" \
    "nosuchcommand-xyz && /bin/echo wrong${NL}nosuchcommand-xyz || /bin/echo recovered" 'recovered'
check      5 "built-ins succeed and fail too" \
    "cd /no/such/dir-xyz && /bin/echo wrong${NL}cd /no/such/dir-xyz || /bin/echo cd-failed${NL}cd / && pwd" \
                                         "cd-failed${NL}/"
check      5 "a pipeline's status is its last command's" \
    "/bin/echo x | false && /bin/echo wrong${NL}/bin/echo x | false || /bin/echo last-failed${NL}cat nosuch.txt | sort && /bin/echo sorted" \
                                         "last-failed${NL}sorted"

# ---------------------------------------------------------------- Bonus
if [[ "$ONLY" == all || "$ONLY" == 6 ]]; then
    echo; echo "Bonus — Ctrl-C and Ctrl-Z (+5 pts)"
fi

check      6 "Ctrl-C ends the command, not the shell" \
    "sleep 5$NL%SLEEP 0.5$NL%INT$NL%SLEEP 0.2$NL/bin/echo alive" 'alive' 3
check      6 "Ctrl-C at the prompt" \
    "/bin/echo one$NL%SLEEP 0.3$NL%INT$NL%SLEEP 0.2$NL/bin/echo two" "one${NL}two" 3
check_jobs 6 "Ctrl-Z stops the command" \
    "sleep 5$NL%SLEEP 0.5$NL%TSTP$NL%SLEEP 0.2$NL/bin/echo back${NL}jobs" "back" 3 \
    '^\[1\].*Stopped.*sleep 5'
check_jobs 6 "Ctrl-C leaves background jobs alone" \
    "sh slow.sh &$NL%SLEEP 0.2$NL%INT$NL%SLEEP 0.8$NL/bin/echo end" "slow${NL}end" 3

# ---------------------------------------------------------------- summary
echo
echo "-------------------------------------------"
pts=0; bonus=0
for l in 1 2 3 4 5 6; do
    [[ "$ONLY" != "all" && "$ONLY" != "$l" ]] && continue
    case $l in 1) w=60;; 2) w=10;; 3) w=10;; 4) w=5;; 5) w=5;; 6) w=5;; esac
    t=${lt[$l]}; p=${lp[$l]}
    earned=$(( w * p / t ))
    if [[ $l -eq 6 ]]; then
        bonus=$earned
        printf 'Bonus:   %2d/%-2d checks   +%d/%d pts\n' "$p" "$t" "$earned" "$w"
    else
        pts=$((pts + earned))
        printf 'Level %s: %2d/%-2d checks   %2d/%d pts\n' "$l" "$p" "$t" "$earned" "$w"
    fi
done
if [[ "$ONLY" == "all" ]]; then
    printf 'Score: %d/90, bonus +%d   (code quality is 10 more and is graded by hand)\n' "$pts" "$bonus"
fi
echo "-------------------------------------------"

[[ $fail -eq 0 ]]
