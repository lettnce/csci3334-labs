// Sum the same 4096 x 4096 matrix of int (64 MB) twice: row by row, then
// column by column. The additions are the same; only the order differs.
//
//   ./02-rowcol

#include <stdio.h>
#include <time.h>

#define N 4096

static int a[N][N];

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

static long sum_rows(void) {
    long sum = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            sum += a[i][j];                   // a[i][0], a[i][1], ... : 4 bytes apart
    return sum;
}

static long sum_cols(void) {
    long sum = 0;
    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
            sum += a[i][j];                   // a[0][j], a[1][j], ... : 16 KB apart
    return sum;
}

int main(void) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            a[i][j] = (i + j) & 7;

    for (int run = 1; run <= 3; run++) {
        double t0 = now();
        long r = sum_rows();
        double t1 = now();
        long c = sum_cols();
        double t2 = now();
        printf("run %d: row-major %6.1f ms, column-major %6.1f ms, %4.1fx%s\n",
               run, (t1 - t0) * 1e3, (t2 - t1) * 1e3, (t2 - t1) / (t1 - t0),
               r == c ? "" : "  (sums differ!)");
    }
    return 0;
}
