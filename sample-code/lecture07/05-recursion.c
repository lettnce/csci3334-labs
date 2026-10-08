// Recursion is not a language feature. It is one frame per call.
//
// Each call gets its own `depth` and its own copy of `pad`, at its own
// address. Printing those addresses shows the stack marching downward,
// one frame at a time.

#include <stdio.h>
#include <stdlib.h>

static long fact(long n, int depth) {
    volatile char pad[32];          // make each frame visibly sized
    (void)pad;

    printf("  depth %d   frame at %p   n = %ld\n",
           depth, (void *)&pad[0], n);

    if (n <= 1) return 1;
    return n * fact(n - 1, depth + 1);
}

int main(int argc, char **argv) {
    long n = (argc > 1) ? strtol(argv[1], NULL, 10) : 5;

    printf("fact(%ld) -- watch the addresses go DOWN:\n\n", n);
    long r = fact(n, 0);
    printf("\nfact(%ld) = %ld\n", n, r);
    puts("\nEach frame is below the one that called it. Return unwinds them");
    puts("in reverse -- and each ret finds its return address exactly where");
    puts("its own call left it.");
    return 0;
}
