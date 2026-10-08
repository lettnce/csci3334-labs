// Every loop becomes a backward jump -- but not the shape you wrote.
//
//     make asm   ->  compare the three functions
//
// A `while` or `for` whose trip count the compiler cannot prove nonzero
// becomes a GUARD plus a do-while: test once up front, then a body that
// always falls into a backward jump. One test per iteration instead of two.

#include <stdio.h>

long sum_while(const long *a, long n) {
    long total = 0;
    long i = 0;
    while (i < n) { total += a[i]; i++; }
    return total;
}

long sum_for(const long *a, long n) {
    long total = 0;
    for (long i = 0; i < n; i++) total += a[i];
    return total;
}

// The compiler knows this runs at least once, so there is no guard at all.
long sum_do(const long *a, long n) {
    long total = 0;
    long i = 0;
    do { total += a[i]; i++; } while (i < n);
    return total;
}

int main(void) {
    long a[] = {1, 2, 3, 4, 5};
    printf("sum_while = %ld\n", sum_while(a, 5));
    printf("sum_for   = %ld\n", sum_for(a, 5));
    printf("sum_do    = %ld\n", sum_do(a, 5));
    puts("\nSame answer. In the assembly, sum_while and sum_for are BYTE-");
    puts("identical. sum_do has no exit guard at all -- it clamps the trip");
    puts("count to at least 1 with cmov, because a do-while always runs once.");
    return 0;
}
