// Multiply two n x n matrices of double, C = A * B, with the three loops in
// three different orders. Every order does the same n^3 multiply-adds and
// gets the same C; only the order of the memory reads changes.
//
//   ./02-matmul            all three orders
//   ./02-matmul kij        one order, for perf stat
//
// n is 1000. Build with -DN=1024 to see what a power of two does to the
// two orders that walk a column.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef N
#define N 1000
#endif

static double A[N][N], B[N][N], C[N][N];   // global, so the stores to C stay

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

static void ijk(void) {                       // inner loop: A by row, B by column
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < N; k++)
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
}

static void jki(void) {                       // inner loop: A and C by column
    for (int j = 0; j < N; j++)
        for (int k = 0; k < N; k++) {
            double r = B[k][j];
            for (int i = 0; i < N; i++)
                C[i][j] += A[i][k] * r;
        }
}

static void kij(void) {                       // inner loop: B and C by row
    for (int k = 0; k < N; k++)
        for (int i = 0; i < N; i++) {
            double r = A[i][k];
            for (int j = 0; j < N; j++)
                C[i][j] += r * B[k][j];
        }
}

static void run(const char *name, void (*f)(void)) {
    memset(C, 0, sizeof C);
    double t0 = now();
    f();
    double t1 = now();
    printf("%s  %7.2f s   %5.2f ns per multiply-add\n",
           name, t1 - t0, (t1 - t0) * 1e9 / ((double)N * N * N));
}

int main(int argc, char **argv) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i][j] = (i + j) % 7;
            B[i][j] = (i * j) % 5;
        }
    const char *only = argc > 1 ? argv[1] : "";
    if (!*only || !strcmp(only, "ijk")) run("ijk", ijk);
    if (!*only || !strcmp(only, "jki")) run("jki", jki);
    if (!*only || !strcmp(only, "kij")) run("kij", kij);
    return 0;
}
