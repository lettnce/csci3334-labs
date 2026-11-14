// Write 1,000,000 bytes to a file three ways and time each:
//   1. write(), one byte per call
//   2. write(), 4096 bytes per call (our own buffer)
//   3. fputc(), one byte per call (stdio's buffer)
//
//   ./03-buffering

#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define N 1000000

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(void) {
    const char *path = "out.bin";
    char c = 'x';

    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    double t0 = now();
    for (int i = 0; i < N; i++)
        if (write(fd, &c, 1) != 1) return 1;
    double t1 = now();
    close(fd);
    printf("write, 1 byte per call:     %7d calls  %8.3f s\n", N, t1 - t0);

    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    char buf[4096];
    int pos = 0, calls = 0;
    t0 = now();
    for (int i = 0; i < N; i++) {
        buf[pos++] = c;
        if (pos == sizeof buf) {
            if (write(fd, buf, pos) != pos) return 1;
            pos = 0;
            calls++;
        }
    }
    if (pos > 0) {                                    // the last partial buffer
        if (write(fd, buf, pos) != pos) return 1;
        calls++;
    }
    t1 = now();
    close(fd);
    printf("write, 4096 bytes per call: %7d calls  %8.3f s\n", calls, t1 - t0);

    FILE *f = fopen(path, "w");
    t0 = now();
    for (int i = 0; i < N; i++)
        fputc(c, f);
    fclose(f);                                        // flushes the buffer
    t1 = now();
    printf("fputc, 1 byte per call:                   %8.3f s\n", t1 - t0);

    unlink(path);
    return 0;
}
