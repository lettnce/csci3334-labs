/*
 * Lab 5 test driver. The harness (run_tests.sh) uses this to run your shell.
 * You do not need to read it, but everything in it is from the process
 * control lectures.
 *
 *     driver <seconds> <script> <program> [args...]
 *
 * It starts <program> (your ./shell) in a process group of its own, the way a
 * terminal starts a foreground job, and feeds it the lines of <script> on
 * stdin through a pipe. A few script lines are instructions for the driver
 * instead of input for the shell:
 *
 *     %SLEEP 0.5    wait half a second before sending the next line
 *     %INT          send SIGINT to the shell's process group (what Ctrl-C does)
 *     %TSTP         send SIGTSTP to the shell's process group (what Ctrl-Z does)
 *     %ZOMBIES      print "driver: zombies=N" on stderr, where N is the number
 *                   of the shell's children that have exited but were never
 *                   reaped with waitpid
 *
 * When the script ends the driver closes the pipe, so the shell sees end of
 * file, and waits for the shell to exit. If it has not exited <seconds> after
 * the start, the driver kills the whole process group, prints
 * "driver: TIMEOUT" on stderr and exits with status 124. Otherwise it exits
 * with the shell's exit status, or 128 + the signal number if a signal killed
 * the shell. Either way it then kills anything still left in the group.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static pid_t shell_pid;

/* If you press Ctrl-C during make test, take the shell under test with us. */
static void on_interrupt(int sig)
{
    if (shell_pid > 0)
        kill(-shell_pid, SIGKILL);
    _exit(128 + sig);
}

static double now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

static void pause_for(double secs)
{
    struct timespec ts = { (time_t)secs, (long)((secs - (time_t)secs) * 1e9) };
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR)
        ;
}

/* Count the children of `parent` that are zombies, using ps. */
static int count_zombies(pid_t parent)
{
    FILE *ps = popen("ps -A -o ppid= -o stat=", "r");
    int n = 0;
    long ppid;
    char stat[32];

    if (!ps)
        return -1;
    while (fscanf(ps, "%ld %31s", &ppid, stat) == 2)
        if (ppid == (long)parent && stat[0] == 'Z')
            n++;
    pclose(ps);
    return n;
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "usage: %s <seconds> <script> <program> [args...]\n", argv[0]);
        return 2;
    }
    double limit = atof(argv[1]);
    FILE *script = fopen(argv[2], "r");
    if (!script) {
        perror(argv[2]);
        return 2;
    }

    int fd[2];
    if (pipe(fd) < 0) {
        perror("pipe");
        return 2;
    }
    signal(SIGPIPE, SIG_IGN);       /* the shell may exit before we finish */

    fflush(stdout);
    fflush(stderr);
    double start = now();
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 2;
    }
    if (pid == 0) {
        setpgid(0, 0);
        /* Start the shell the way a terminal would: default signal actions
           and nothing blocked. A command started with & from a script
           inherits SIGINT ignored, and that would hide a shell that
           Ctrl-C kills. */
        int sigs[] = { SIGINT, SIGQUIT, SIGTSTP, SIGTTIN, SIGTTOU, SIGCHLD,
                       SIGPIPE, SIGHUP, SIGTERM, SIGALRM };
        for (size_t i = 0; i < sizeof sigs / sizeof sigs[0]; i++)
            signal(sigs[i], SIG_DFL);
        sigset_t none;
        sigemptyset(&none);
        sigprocmask(SIG_SETMASK, &none, NULL);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        fclose(script);
        execv(argv[3], &argv[3]);
        perror(argv[3]);
        _exit(127);
    }
    setpgid(pid, pid);               /* also here, so there is no race */
    shell_pid = pid;
    signal(SIGINT, on_interrupt);
    signal(SIGTERM, on_interrupt);
    signal(SIGHUP, on_interrupt);
    close(fd[0]);

    char line[1024];
    while (fgets(line, sizeof line, script)) {
        if (strncmp(line, "%SLEEP", 6) == 0) {
            pause_for(atof(line + 6));
        } else if (strncmp(line, "%INT", 4) == 0) {
            kill(-pid, SIGINT);
        } else if (strncmp(line, "%TSTP", 5) == 0) {
            kill(-pid, SIGTSTP);
        } else if (strncmp(line, "%ZOMBIES", 8) == 0) {
            fprintf(stderr, "driver: zombies=%d\n", count_zombies(pid));
            fflush(stderr);
        } else {
            size_t len = strlen(line);
            if (write(fd[1], line, len) != (ssize_t)len)
                break;               /* the shell is gone */
        }
    }
    fclose(script);
    close(fd[1]);                    /* end of file for the shell */

    int status = 0, timed_out = 0;
    for (;;) {
        pid_t r = waitpid(pid, &status, WNOHANG);
        if (r == pid)
            break;
        if (r < 0 && errno != EINTR)
            break;
        if (now() - start > limit) {
            kill(-pid, SIGKILL);
            waitpid(pid, &status, 0);
            timed_out = 1;
            break;
        }
        pause_for(0.01);
    }
    kill(-pid, SIGKILL);             /* clean up anything left in the group */

    if (timed_out) {
        fprintf(stderr, "driver: TIMEOUT\n");
        return 124;
    }
    if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}
