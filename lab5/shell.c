/*
 * Lab 5 — Simple Shell
 *
 * Work through the levels in order. Each one is tested and scored on its own:
 *
 *     make test          run every level
 *     make test L=1      run only Level 1
 *     make test L=bonus  run only the bonus
 *
 *   Level 1 (60 pts)  run commands; the built-ins cd, pwd and exit
 *   Level 2 (10 pts)  redirection: >  >>  <  2>  2>&1
 *   Level 3 (10 pts)  pipelines: cmd1 | cmd2 | ... | cmdN
 *   Level 4  (5 pts)  background jobs: cmd &, jobs, fg %n
 *   Level 5  (5 pts)  && and ||: run the next command only if the last
 *                     one succeeded (&&) or failed (||)
 *   Code quality (10 pts, graded by hand): no warnings, no leaks, readable
 *   Bonus   (+5 pts)  Ctrl-C and Ctrl-Z reach the command, not the shell
 *
 * Level 1 alone is a passing grade on this lab. Get it fully green before
 * you start Level 2. The later levels are built on run_external(), and
 * debugging them on top of a shaky Level 1 is miserable.
 *
 * Everything else is already wired up: the main loop runs, the prompt
 * prints, and the program compiles and exits cleanly. Your job is to fill in
 * the functions marked YOUR CODE HERE, one at a time.
 */

/* strdup() is POSIX, not ISO C. This line makes it (and the other POSIX
   functions) visible even though we build with -std=c17. */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

/* Forward declarations, so you can call any of these from any other one
   regardless of the order they appear in the file. In C a function must be
   declared before it is used, and Level 2's apply_redirection() has to be
   called from Level 1's run_external(). */
static int run_builtin(char *argv[]);
static int run_external(char *argv[]);
static int apply_redirection(char *argv[]);
static int run_pipeline(char **cmds[], int n);

/* ------------------------------------------------------------------ *
 * Provided for you. Read these; you will need to call them.
 * ------------------------------------------------------------------ */

/* Print the prompt, but only when stdin is a terminal, so the test harness
   can pipe commands in without the prompt getting into the output. */
static void print_prompt(void)
{
    if (isatty(STDIN_FILENO)) {
        printf("$ ");
        fflush(stdout);
    }
}

/* Read one line from stdin. Returns a malloc'd string with the trailing
   newline stripped, or NULL at end of input. The caller frees it. */
static char *read_line(void)
{
    char buf[MAX_LINE];

    if (fgets(buf, sizeof buf, stdin) == NULL)
        return NULL;

    buf[strcspn(buf, "\n")] = '\0';
    return strdup(buf);
}

/* Split `line` on spaces and tabs into `argv`, NULL-terminated.
   Returns the number of words. It writes '\0' into `line` itself, so the
   words point into `line`, and `line` no longer reads as the whole command.
   There is no quoting: "a b" is two words, "a and b". */
static int tokenize(char *line, char *argv[], int max_args)
{
    int argc = 0;

    for (char *tok = strtok(line, " \t"); tok && argc < max_args - 1;
         tok = strtok(NULL, " \t"))
        argv[argc++] = tok;

    argv[argc] = NULL;
    return argc;
}

/* ------------------------------------------------------------------ *
 * LEVEL 1 (60 pts) — Running commands and built-ins
 * ------------------------------------------------------------------ */

/*
 * Handle a built-in command. Return 1 if `argv[0]` was a built-in and it was
 * handled, 0 if it is not a built-in (so the caller should fork and exec).
 *
 * Built-ins required:
 *   exit          end the shell (exit status 0)
 *   cd [dir]      chdir(dir); with no argument, chdir to getenv("HOME")
 *   pwd           print the current working directory
 *
 * These have to run inside the shell itself. A cd done in a child process
 * changes the child's directory, and the child then exits.
 *
 * On an error (for example cd to a directory that does not exist) print a
 * message to stderr with perror() and still return 1. A failed built-in is
 * still a built-in.
 *
 * Hint: getcwd(NULL, 0) mallocs a buffer of the right size for you. Free it.
 */
static int run_builtin(char *argv[])
{
    /* YOUR CODE HERE */
    (void)argv;
    return 0;
}

/*
 * Run an external command and wait for it to finish.
 * Return the child's status, as waitpid() reports it.
 *
 * Steps:
 *   1. fflush(stdout), so nothing the shell has printf'd is still sitting
 *      in its buffer when fork() copies the process
 *   2. fork()
 *   3. in the child:  execvp(argv[0], argv)
 *                     if execvp returns, it FAILED: perror(argv[0]) and
 *                     _exit(127). Never return from the child, and use
 *                     _exit, not exit (the handout says why).
 *   4. in the parent: waitpid(pid, &status, 0) for THAT child, return status
 *
 * execvp searches PATH for you when argv[0] has no '/' in it.
 */
static int run_external(char *argv[])
{
    /* YOUR CODE HERE */
    (void)argv;
    return 0;
}

/* ------------------------------------------------------------------ *
 * LEVEL 2 (10 pts) — I/O redirection
 * ------------------------------------------------------------------ */

/*
 * Scan argv for redirection operators, apply them, and REMOVE them (and
 * their file names) from argv, so the command itself never sees them.
 *
 *   > file     stdout to file   (create or truncate, mode 0644)
 *   >> file    stdout to file   (create or append)
 *   < file     stdin from file
 *   2> file    stderr to file   (create or truncate)
 *   2>&1       stderr to wherever stdout points at that moment
 *
 * Work from left to right, so "> f 2>&1" and "2>&1 > f" differ the way
 * they do in bash. Each operator is a separate word ("> out.txt",
 * not ">out.txt").
 *
 * Call this in the CHILD, after fork() and before execvp(). If it returns
 * -1, the child must print nothing more and _exit(1) without running the
 * command. Return 0 on success, -1 on an error (for example, the input file
 * does not exist: perror(file) and return -1).
 *
 * Hint: open() the file, dup2(fd, STDOUT_FILENO), then close(fd).
 * Hint: copy each word you keep down to the next free slot in argv, skip the
 *       operators and file names, and put NULL after the last word you kept.
 */
static int apply_redirection(char *argv[])
{
    /* YOUR CODE HERE */
    (void)argv;
    return 0;
}

/* ------------------------------------------------------------------ *
 * LEVEL 3 (10 pts) — Pipes
 * ------------------------------------------------------------------ */

/*
 * Run a pipeline of `n` commands: cmds[0] | cmds[1] | ... | cmds[n-1].
 * Each cmds[i] is a NULL-terminated argv array, and may contain
 * redirections, so call apply_redirection(cmds[i]) in each child.
 * Return the status of the LAST command.
 *
 * Sketch:
 *   for each command i:
 *       if not the last, pipe(fd)
 *       fork()
 *       child:  if there is a previous read end, dup2 it onto stdin
 *               if not the last, dup2(fd[1]) onto stdout
 *               close every pipe descriptor it still has open, then exec
 *       parent: close the previous read end and this pipe's write end,
 *               and remember this pipe's read end for the next command
 *   then waitpid() for all n children, not just the last one.
 *
 * The single most common bug: forgetting to close unused pipe ends in the
 * parent. A reader sees end of file only when EVERY write end is closed, so
 * if the shell keeps one, the pipeline hangs.
 */
static int run_pipeline(char **cmds[], int n)
{
    /* YOUR CODE HERE */
    (void)cmds;
    (void)n;
    return 0;
}

/* ------------------------------------------------------------------ *
 * LEVEL 4 (5 pts) — Background jobs:  cmd &,  jobs,  fg %n
 * LEVEL 5 (5 pts) — && and ||
 * BONUS  (+5 pts) — Ctrl-C and Ctrl-Z reach the command, not the shell
 *
 * Add your own functions here once Levels 1-3 are green. The handout's
 * Level 4, Level 5 and Bonus sections give the exact output and a plan.
 *
 * Level 5 needs each command's exit status: WIFEXITED(status) and
 * WEXITSTATUS(status) turn what waitpid() reported into 0 (success) or a
 * failure code. A built-in has a status too (cd fails when chdir fails), so
 * keep it in a variable of your own; run_builtin's return value already
 * means "was it a built-in".
 *
 * This struct is one way to keep the Level 4 jobs table; declare the array
 * yourself:
 *
 *     static Job jobs[MAX_JOBS];
 * ------------------------------------------------------------------ */

#define MAX_JOBS 32

typedef struct {
    int   id;         /* the job number that jobs and fg use; 0 = free slot */
    pid_t pid;
    int   stopped;    /* bonus: 1 if Ctrl-Z stopped it, 0 if it is running */
    char *cmd;        /* malloc'd copy of the command, for jobs to print */
} Job;

/* ------------------------------------------------------------------ *
 * Main loop. Provided; you will add to it at Levels 3, 4 and 5 and for
 * the bonus.
 * ------------------------------------------------------------------ */

int main(void)
{
    char *line;

    /* Bonus: install your signal handlers here, before the loop. */

    /* Keeps the compiler quiet about the Level 2 and 3 stubs you have not
       wired up yet. Delete these two lines once you call them for real. */
    (void)apply_redirection;
    (void)run_pipeline;

    while (print_prompt(), (line = read_line()) != NULL) {
        char *argv[MAX_ARGS];
        int argc;

        /* Level 4: first reap every background job that has finished
           (waitpid with WNOHANG, in a loop), so none is left a zombie. */

        argc = tokenize(line, argv, MAX_ARGS);
        if (argc == 0) {            /* a blank line */
            free(line);
            continue;
        }

        /* Level 4: if the last word is "&", remove it and run the command
           in the background instead.
           Level 3: if any word is "|", split argv into commands and call
           run_pipeline() instead.
           Level 5: if any word is "&&" or "||", cut the line there and
           run the pieces from left to right, each one only if the status
           of the last piece that ran allows it. */

        if (!run_builtin(argv))
            run_external(argv);

        free(line);
    }

    return 0;
}
