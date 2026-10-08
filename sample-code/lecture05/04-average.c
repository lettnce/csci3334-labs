/* Lecture 5: the overflow that hid in binary search for nine years.
 *
 * (lo + hi) / 2 is the obvious midpoint and it is wrong: the sum can leave
 * the range even though the average never does. This was in Java's standard
 * library, and in Programming Pearls, until 2006.
 */
#include <limits.h>
#include <stdio.h>

static int mid_broken(int lo, int hi) { return (lo + hi) / 2; }
static int mid_ok(int lo, int hi)     { return lo + (hi - lo) / 2; }

int main(void)
{
    int lo = 1500000000, hi = 2000000000;

    printf("lo = %d   hi = %d\n", lo, hi);
    printf("the true midpoint is  %lld\n\n", ((long long)lo + hi) / 2);

    /* lo + hi is 3.5e9, past INT_MAX (%d) — signed overflow, undefined.
       What it prints here is what this compiler happened to do today. */
    printf("(lo + hi) / 2        %d   <- lo + hi overflowed\n", mid_broken(lo, hi));
    printf("lo + (hi - lo) / 2   %d   <- hi - lo is small, so nothing overflows\n",
           mid_ok(lo, hi));

    printf("\nINT_MAX is %d; lo + hi wanted %lld\n",
           INT_MAX, (long long)lo + hi);
    return 0;
}
