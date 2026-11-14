// Ask read for 4096 bytes at a time and print how many it actually returns.
// From a pipe, read returns whatever has arrived so far.
//
//   (echo hello; sleep 1; echo world) | ./02-short-read
//   ./02-short-read < 02-short-read.c

#include <stdio.h>
#include <unistd.h>

int main(void) {
    char buf[4096];
    ssize_t n;
    do {
        n = read(STDIN_FILENO, buf, sizeof buf);
        printf("asked for %zu, read returned %zd\n", sizeof buf, n);
    } while (n > 0);
    return 0;
}
