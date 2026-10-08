// Address space layout randomisation, seen from the inside.
//
// Run this twice. Every address changes -- which is exactly what makes a
// hard-coded target address useless to an attacker.

#include <stdio.h>
#include <stdlib.h>

static int global = 1;

static void a_function(void) { }

int main(void) {
    int local;
    void *heap = malloc(16);

    printf("stack (a local)   %p\n", (void *)&local);
    printf("heap  (malloc)    %p\n", heap);
    printf("code  (function)  %p\n", (void *)a_function);
    printf("data  (a global)  %p\n", (void *)&global);

    free(heap);
    puts("\nRun it again. Compare. Anything that moved is randomised per run.");
    return 0;
}
