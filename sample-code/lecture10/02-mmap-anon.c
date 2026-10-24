// An anonymous mapping: 1 MB of fresh, zeroed memory straight from the kernel.
// This is how malloc gets large blocks (Lecture 11).

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

int main(void) {
    size_t len = 1 << 20;
    char *mem = mmap(NULL, len, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) { perror("mmap"); exit(1); }

    printf("mapped 1 MB at %p\n", (void *)mem);
    printf("mem[12345] = %d (the kernel zeroes new memory)\n", mem[12345]);
    mem[12345] = 42;
    printf("mem[12345] = %d after a write\n", mem[12345]);

    munmap(mem, len);
    return 0;
}
