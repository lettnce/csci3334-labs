/* Lecture 5: signed types promote first, then get truncated on the way back. */
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

/* WRONG: the overflow has already happened by the time the test runs, and the
   compiler is entitled to assume it never does — so it may delete the test. */
static int broken_check(int x, int y)
{
    int sum = x + y;
    return x > 0 && y > 0 && sum < 0;
}

/* RIGHT: test the operands, so the overflow never happens. */
static int will_overflow(int x, int y)
{
    if (y > 0 && x > INT_MAX - y) return 1;
    if (y < 0 && x < INT_MIN - y) return 1;
    return 0;
}

int main(void)
{
    int8_t a = 100, b = 50;
    int    promoted = a + b;          /* computed as int: exactly 150 */
    int8_t squeezed = (int8_t)(a + b);

    printf("100 + 50 as int      %d\n", promoted);
    printf("...back into int8_t  %d   <- the conversion changed it, not the add\n\n",
           squeezed);

    printf("INT_MAX          %d\n", INT_MAX);
    printf("will_overflow(INT_MAX, 1)   %d\n", will_overflow(INT_MAX, 1));
    printf("will_overflow(INT_MAX, 0)   %d\n", will_overflow(INT_MAX, 0));
    printf("will_overflow(INT_MIN, -1)  %d\n\n", will_overflow(INT_MIN, -1));

    /* Multiplication has the same trap, and widening the destination does
       not help: by then the 32-bit multiply has already overflowed. Widen an
       operand instead -- one is enough, because the usual arithmetic
       conversions bring the other up to match. */
    int32_t m = 100000, n = 100000;
    int64_t widened_late  = m * n;
    int64_t widened_early = (int64_t)m * n;
    printf("int64_t x = m * n;            -> %lld   (32-bit multiply, then widened)\n",
           (long long)widened_late);
    printf("int64_t x = (int64_t)m * n;   -> %lld\n\n",
           (long long)widened_early);

    /* What production code uses. */
    int sum;
    if (__builtin_add_overflow(INT_MAX, 1, &sum))
        printf("__builtin_add_overflow said no, and never computed it\n");

    (void)broken_check;
    return 0;
}
