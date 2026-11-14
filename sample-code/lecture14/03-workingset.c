// How long one load takes when the data fits in L1, L2, L3, or only in
// main memory. The program builds a ring of nodes in a random order, one
// node per 128 bytes, and follows the ring. Each load needs the address
// from the load before it, so the CPU cannot overlap them, and the random
// order defeats the hardware prefetcher.
//
//   ./03-workingset

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NODE 128                              // bytes per node: one node per cache line
#define LOADS (1L << 24)                      // loads timed at every size

struct node { struct node *next; char pad[NODE - sizeof(struct node *)]; };

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(void) {
    printf("working set  ns per load\n");
    for (long size = 16L << 10; size <= 256L << 20; size *= 2) {
        long n = size / NODE;
        struct node *v = malloc(n * sizeof *v);
        long *order = malloc(n * sizeof *order);
        if (v == NULL || order == NULL) { perror("malloc"); return 1; }

        for (long i = 0; i < n; i++)
            order[i] = i;
        for (long i = n - 1; i > 0; i--) {    // shuffle the visiting order
            long j = random() % (i + 1);
            long t = order[i]; order[i] = order[j]; order[j] = t;
        }
        for (long i = 0; i < n; i++)          // link the nodes into one ring
            v[order[i]].next = &v[order[(i + 1) % n]];

        struct node *p = &v[order[0]];
        for (long i = 0; i < LOADS; i++)      // warm up: same number of loads, untimed
            p = p->next;
        double t0 = now();
        for (long i = 0; i < LOADS; i++)
            p = p->next;
        double t1 = now();

        if (size < (1L << 20))
            printf("%7ld KB  %11.1f%s\n", size >> 10, (t1 - t0) * 1e9 / LOADS,
                   p == NULL ? "!" : "");     // use p, so the loop stays
        else
            printf("%7ld MB  %11.1f%s\n", size >> 20, (t1 - t0) * 1e9 / LOADS,
                   p == NULL ? "!" : "");
        free(v);
        free(order);
    }
    return 0;
}
