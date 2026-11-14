// Sum a 4096 x 4096 matrix of int column by column, twice. The first matrix
// has rows of exactly 4096 ints (16 KB). The second has 16 unused ints at the
// end of every row, so its rows are 16 KB + 64 bytes long. The additions are
// the same; only the distance between a[i][j] and a[i+1][j] changes.
//
//   ./01-conflict

#include <stdio.h>
#include <time.h>

#define N 4096
#define PAD 16                                // 16 ints = one 64-byte block

static int a[N][N];                           // rows 16 KB apart
static int p[N][N + PAD];                     // rows 16 KB + 64 bytes apart

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

static long cols_a(void) {
    long sum = 0;
    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
            sum += a[i][j];                   // 16384 bytes apart
    return sum;
}

static long cols_p(void) {
    long sum = 0;
    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
            sum += p[i][j];                   // 16448 bytes apart
    return sum;
}

int main(void) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            a[i][j] = p[i][j] = (i + j) & 7;

    for (int run = 1; run <= 3; run++) {
        double t0 = now();
        long x = cols_a();
        double t1 = now();
        long y = cols_p();
        double t2 = now();
        printf("run %d: rows 16 KB apart %6.1f ms, rows 16 KB + 64 B apart %6.1f ms, %4.1fx%s\n",
               run, (t1 - t0) * 1e3, (t2 - t1) * 1e3, (t1 - t0) / (t2 - t1),
               x == y ? "" : "  (sums differ!)");
    }
    return 0;
}
