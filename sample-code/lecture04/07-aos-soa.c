/* Lecture 4: the same records, two layouts, one very different walk.
 *
 * Array of Structs keeps each record's fields together. Struct of Arrays keeps
 * each field's values together. If you touch every field of one record at a
 * time, AoS wins. If you touch one field of every record — which is what
 * almost every analytics query and every tensor kernel does — SoA wins,
 * because it is the only one of the two that reads memory without gaps.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 8000000

struct Particle {           /* 32 bytes */
    double x, y, z;
    int    id;
};

static double now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(void)
{
    struct Particle *aos = malloc(N * sizeof *aos);
    double *xs = malloc(N * sizeof *xs);
    if (!aos || !xs)
        return 1;
    for (int i = 0; i < N; i++)
        aos[i].x = xs[i] = 1.0;

    double t0 = now(), sum_aos = 0.0;
    for (int i = 0; i < N; i++)          /* 32 bytes apart */
        sum_aos += aos[i].x;
    double t1 = now(), sum_soa = 0.0;
    for (int i = 0; i < N; i++)          /* 8 bytes apart  */
        sum_soa += xs[i];
    double t2 = now();

    printf("sizeof(struct Particle) = %zu bytes\n", sizeof(struct Particle));
    printf("summing x over %d particles\n\n", N);
    printf("array of structs  %5.1f ms   stride %2zu bytes   %4.0f MB pulled in\n",
           (t1 - t0) * 1e3, sizeof(struct Particle),
           (double)N * sizeof(struct Particle) / 1e6);
    printf("struct of arrays  %5.1f ms   stride %2zu bytes   %4.0f MB pulled in\n",
           (t2 - t1) * 1e3, sizeof(double), (double)N * sizeof(double) / 1e6);
    printf("\nsame answer (%g) both ways, %.1fx apart in time.\n",
           sum_aos == sum_soa ? sum_aos : -1.0, (t1 - t0) / (t2 - t1));
    printf("Four times the bytes, but not four times the time: the hardware\n");
    printf("prefetcher hides part of a regular stride. Lecture 16 explains how.\n");

    free(aos);
    free(xs);
    return 0;
}
