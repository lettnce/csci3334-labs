// Two threads each add 1 to their own counter 200 million times. In the
// first run the two counters are next to each other, in one 64-byte block.
// In the second run they are 64 bytes apart, in two different blocks. The
// threads never touch each other's counter.
//
//   ./03-falseshare

#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define ADDS 200000000L

static _Alignas(64) volatile long counters[16]; // 128 bytes, starts on a block

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

static void *work(void *arg) {
    volatile long *c = arg;
    for (long i = 0; i < ADDS; i++)
        *c += 1;                              // a load and a store every time
    return NULL;
}

// Run two threads on counters[0] and counters[other]; return the seconds.
static double race(int other) {
    pthread_t t1, t2;
    counters[0] = counters[other] = 0;
    double t0 = now();
    pthread_create(&t1, NULL, work, (void *)&counters[0]);
    pthread_create(&t2, NULL, work, (void *)&counters[other]);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return now() - t0;
}

int main(void) {
    for (int run = 1; run <= 3; run++) {
        double same = race(1);                // 8 bytes apart: same block
        double apart = race(8);               // 64 bytes apart: two blocks
        printf("run %d: same block %5.2f s, different blocks %5.2f s, %4.1fx\n",
               run, same, apart, same / apart);
    }
    return 0;
}
