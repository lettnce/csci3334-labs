// Lecture 9's page-fault program, with the loop run twice. The first pass
// faults once per page; the second pass faults on none, because every page
// table entry now has Present set.
//
//   ./01-page-faults-twice           (Linux gives exact counts; macOS counts differently)

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

#define BYTES (64L << 20)   // 64 MiB
#define PAGE  4096

static long minor_faults(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_minflt;
}

int main(void) {
    volatile char *p = malloc(BYTES);
    if (!p) { perror("malloc"); return 1; }

    long before = minor_faults();
    for (long i = 0; i < BYTES; i += PAGE)
        p[i] = 1;                         // first write to each page
    long first = minor_faults() - before;

    before = minor_faults();
    for (long i = 0; i < BYTES; i += PAGE)
        p[i] = 2;                         // same pages again
    long second = minor_faults() - before;

    printf("pages touched      %ld\n", BYTES / PAGE);
    printf("first pass faults  %ld\n", first);
    printf("second pass faults %ld\n", second);
    free((void *)p);
    return 0;
}
