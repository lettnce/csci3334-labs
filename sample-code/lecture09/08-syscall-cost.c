// What does it cost to cross into the kernel and back?
//
// Times one million calls to three things:
//   1. an ordinary function that does almost nothing
//   2. getppid(), a system call that does almost nothing
//   3. write() of one byte to /dev/null, a system call that does a little
//
// The work inside each is tiny, so the time measured is mostly the cost of
// the call itself. Numbers vary from machine to machine; the ratio is the point.

#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define N 1000000

// noinline so the compiler really makes a call, and volatile so it cannot
// decide the loop does nothing and delete it.
__attribute__((noinline)) static int tiny(int x) { return x + 1; }

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

int main(void) {
    volatile int sink = 0;
    int fd = open("/dev/null", O_WRONLY);
    double t0, per_call;

    t0 = now();
    for (int i = 0; i < N; i++) sink = tiny(sink);
    per_call = (now() - t0) / N * 1e9;
    printf("function call       %7.1f ns each\n", per_call);

    t0 = now();
    for (int i = 0; i < N; i++) sink = getppid();
    per_call = (now() - t0) / N * 1e9;
    printf("getppid()           %7.1f ns each\n", per_call);

    t0 = now();
    for (int i = 0; i < N; i++) sink = (int)write(fd, "x", 1);
    per_call = (now() - t0) / N * 1e9;
    printf("write 1 byte        %7.1f ns each\n", per_call);

    close(fd);
    return 0;
}
