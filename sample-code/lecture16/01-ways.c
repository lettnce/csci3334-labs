// How many blocks can share one set? The program puts k nodes exactly
// 4096 bytes apart, so on a cache that picks the set from address bits 6 to
// 11 they all land in the same set. It links the nodes into a ring and
// follows it over and over: node 0, 1, ..., k-1, node 0 again. Each load
// needs the address from the load before, so the time per load is the time
// of one read from wherever the node is.
//
// The second column does the same with the nodes 4096 + 64 bytes apart,
// which puts each one in a different set.
//
//   ./01-ways

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXK 24
#define LOADS (1L << 26)

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

// Build a ring of k nodes, gap bytes apart, follow it LOADS times, and
// return the nanoseconds per load.
static double chase(char *buf, int k, long gap) {
    for (int i = 0; i < k; i++)
        *(char **)(buf + i * gap) = buf + ((i + 1) % k) * gap;
    char *q = buf;
    for (long i = 0; i < LOADS / 8; i++)      // warm up, untimed
        q = *(char **)q;
    double t0 = now();
    for (long i = 0; i < LOADS; i++)
        q = *(char **)q;
    double t1 = now();
    if (q == NULL)                            // use q, so the loop stays
        printf("!");
    return (t1 - t0) * 1e9 / LOADS;
}

int main(void) {
    long size = (MAXK * 4160L + 4095) / 4096 * 4096;  // a multiple of 4096
    char *buf = aligned_alloc(4096, size);
    if (buf == NULL) { perror("aligned_alloc"); return 1; }
    printf(" k   same set (4096 apart)   different sets (4160 apart)\n");
    for (int k = 1; k <= MAXK; k++)
        printf("%2d   %10.2f ns          %10.2f ns\n",
               k, chase(buf, k, 4096), chase(buf, k, 4160));
    free(buf);
    return 0;
}
