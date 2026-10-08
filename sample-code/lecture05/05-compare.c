/* Lecture 5: when signed meets unsigned, one of them converts — usually not
 * the one you hoped.
 *
 * Note what happens if you delete the pragma below: -Wall turns on
 * -Wsign-compare, and the compiler refuses to build this file. That warning
 * is the single best defence against everything on this slide. The pragma is
 * here so you can see the bug; never write one in real code.
 */
#include <stdio.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

static int lt(int a, unsigned b)   { return a < b; }
static int lt_wide(int a, unsigned b) { return (long)a < (long)b; }

/* The real bug class: the bounds check is signed, the use is unsigned. A
   negative n sails past "n > cap" and then becomes enormous inside memcpy. */
static void copy_into(size_t cap, int n)
{
    if (n > (int)cap) {
        printf("rejected: %d is more than %zu\n", n, cap);
        return;
    }
    printf("accepted: memcpy would be asked for %zu bytes\n", (size_t)n);
}

#pragma GCC diagnostic pop

int main(void)
{
    int x = -1;
    unsigned int y = 1;

    printf("x = %d, y = %u\n", x, y);
    printf("x < y            -> %-5s  <- x is converted to %u first\n",
           lt(x, y) ? "true" : "false", (unsigned)x);
    printf("(long)x < (long)y -> %-5s  <- long holds every unsigned int value\n\n",
           lt_wide(x, y) ? "true" : "false");

    printf("copy_into(cap = 10, n = 20)   ");
    copy_into(10, 20);
    printf("copy_into(cap = 10, n = -1)   ");
    copy_into(10, -1);
    printf("\n");

    /* size_t is unsigned, so len - 1 on an empty container is enormous. */
    size_t len = 0;
    printf("len = 0,  len - 1 = %zu\n", len - 1);
    return 0;
}
