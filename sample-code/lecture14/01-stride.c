// Add up every int in a 128 MB array, in different orders. Stride s means
// the loop reads a[0], a[s], a[2s], ..., then starts again at a[1], and so
// on, s passes in all. Every stride does the same additions on the same
// bytes; only the order changes.
//
//   ./01-stride

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N (1L << 25)                         // 32 Mi ints = 128 MB

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(void) {
    int *a = malloc(N * sizeof *a);
    if (a == NULL) { perror("malloc"); return 1; }
    for (long i = 0; i < N; i++)              // touch every page first
        a[i] = i & 7;

    long total = 0;
    for (long i = 0; i < N; i++)              // one untimed pass to warm up
        total += a[i];
    printf("stride  bytes apart  ns per int\n");
    for (long s = 1; s <= 64; s *= 2) {
        long sum = 0;
        double t0 = now();
        for (long r = 0; r < s; r++)          // s passes
            for (long i = r; i < N; i += s)   // each pass reads every s-th int
                sum += a[i];
        double t1 = now();
        total += sum;
        printf("%6ld  %11ld  %10.2f\n", s, s * (long)sizeof(int),
               (t1 - t0) * 1e9 / N);
    }
    printf("(sum %ld)\n", total);             // use the sums, so the loops stay
    free(a);
    return 0;
}
