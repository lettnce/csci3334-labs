// Undefined behaviour is not "it does something weird at runtime".
// It is "the compiler assumes it cannot happen, and optimises on that".
//
// The point of this program is that THE ANSWER CHANGES WITH -O.
// Build it both ways and run both:
//
//     make ub
//
//     -O0  check_overflow(INT_MAX) = 0     the comparison actually runs
//     -O1  check_overflow(INT_MAX) = 1     the comparison is GONE
//
// Same source, same machine, same run. Only the optimiser differs.

#include <stdio.h>
#include <limits.h>

// The classic broken overflow check. Signed overflow is UB, so the compiler
// is entitled to assume x + 1 never wraps -- which makes x + 1 > x always
// true, which makes the test dead code. At -O1 this becomes `movl $1, %eax`.
int check_overflow(int x) {
    return x + 1 > x;
}

// The same question, asked in a way that is DEFINED. Survives every -O level.
int check_overflow_ok(int x) {
    return x != INT_MAX;
}

// Unsigned wraparound is fully defined, so this one is never removed.
int check_unsigned(unsigned x) {
    return x + 1 > x;
}

int main(void) {
#ifdef __OPTIMIZE__
    puts("built WITH optimisation (-O1 or higher)");
#else
    puts("built WITHOUT optimisation (-O0)");
#endif

    printf("\n  check_overflow(INT_MAX)    = %d\n", check_overflow(INT_MAX));
    printf("  check_overflow_ok(INT_MAX) = %d\n", check_overflow_ok(INT_MAX));
    printf("  check_unsigned(UINT_MAX)   = %d\n", check_unsigned(UINT_MAX));

    puts("\n  The correct answer is 0 for all three.");
    puts("  If the first one printed 1, the check was deleted as dead code.");
    return 0;
}
