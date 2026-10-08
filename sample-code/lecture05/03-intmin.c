/* Lecture 5: two's complement is not symmetric, and INT_MIN pays for it.
 *
 * With 32 bits there are 2^32 patterns and an odd number of them left over
 * once you have zero. The range runs -2147483648 .. +2147483647, so INT_MIN
 * has no positive twin — and every operation that wants one is undefined.
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    printf("INT_MIN  %d\n", INT_MIN);
    printf("INT_MAX  %d\n", INT_MAX);
    printf("|INT_MIN| would be %lld, which is one past INT_MAX\n\n",
           -(long long)INT_MIN);

    /* All three of these are undefined behavior on a 32-bit int:
     *     -INT_MIN            abs(INT_MIN)            INT_MIN / -1
     * The last one usually traps rather than wrapping — the hardware's
     * divide instruction raises an exception it cannot represent. */
    printf("-(long long)INT_MIN  %lld   <- widen first, and it is fine\n",
           -(long long)INT_MIN);
    printf("llabs((long long)INT_MIN)  %lld\n\n", llabs((long long)INT_MIN));

    /* The bit pattern is its own negation: flip and add one, and you are
       back where you started. That is the whole reason. */
    unsigned u = (unsigned)INT_MIN;
    printf("INT_MIN as bits  0x%08X\n", u);
    printf("~x + 1           0x%08X   <- the same pattern\n", ~u + 1u);
    return 0;
}
