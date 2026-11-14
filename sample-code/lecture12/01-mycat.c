// Copy a file (or stdin) to stdout with nothing but open, read, write and
// close. Run it under strace on Linux to see one system call per step.
//
//   ./01-mycat 01-mycat.c
//   echo hello | ./01-mycat

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int fd = STDIN_FILENO;                    // no argument: read stdin
    if (argc == 2) {
        fd = open(argv[1], O_RDONLY);
        if (fd < 0) { perror(argv[1]); return 1; }
    }

    char buf[4096];
    ssize_t n;
    while ((n = read(fd, buf, sizeof buf)) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) { perror("write"); return 1; }
    }
    if (n < 0) { perror("read"); return 1; }  // n == 0 means end of file

    if (fd != STDIN_FILENO) close(fd);
    return 0;
}
