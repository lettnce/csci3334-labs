// Page faults are normal. Count them.
//
// malloc gives us address space, but the kernel does not attach real memory to
// a page until the program first touches it. The first touch of each page
// raises a page fault, the kernel supplies a zeroed page, and the instruction
// runs again. The program never notices, except in this count.
//
// getrusage reports "minor" faults: ones the kernel fixed without reading the
// disk. Expect about one per page touched.

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

#define MB (1024L * 1024L)

static long minor_faults(void) {
    struct rusage u;
    getrusage(RUSAGE_SELF, &u);
    return u.ru_minflt;
}

int main(void) {
    long size = 64 * MB;
    long page = sysconf(_SC_PAGESIZE);
    volatile char *p = malloc(size);    // volatile: every write must really happen
    if (p == NULL) { perror("malloc"); return 1; }

    long before = minor_faults();
    for (long i = 0; i < size; i += page)
        p[i] = 1;                       // one write per page: the first touch
    long after = minor_faults();

    printf("page size          %ld bytes\n", page);
    printf("pages touched      %ld\n", size / page);
    printf("minor page faults  %ld\n", after - before);
    free((void *)p);
    return 0;
}
