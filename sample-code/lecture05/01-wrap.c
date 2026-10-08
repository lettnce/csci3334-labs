/* Lecture 5: unsigned arithmetic wraps, and the size of the box decides. */
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    uint8_t a = 200, b = 100;

    uint8_t  small = a + b;      /* squeezed back into 8 bits  */
    uint16_t big   = a + b;      /* the same sum, given 16 bits */

    printf("200 + 100 into a uint8_t   %3u   (300 mod 256)\n", small);
    printf("200 + 100 into a uint16_t  %3u\n\n", big);

    /* Wrapping is defined for unsigned types, so you may rely on it. */
    uint8_t x = 0;
    printf("0 - 1 as uint8_t   %u\n", (unsigned)(uint8_t)(x - 1));
    /* An unsigned counter is never negative, so this loop never ends:
     *     for (size_t i = len - 1; i >= 0; i--)
     * GNU gcc -Wextra catches it ("comparison ... is always true"). Apple's
     * clang — which is what `gcc` runs on a Mac — says nothing unless you add
     * -Wtype-limits. Count down like this instead: */
    size_t len = 4;
    printf("counting down from %zu:", len);
    for (size_t i = len; i-- > 0; )
        printf(" %zu", i);
    printf("\n");

    /* Same story one size up. */
    uint32_t u = 4294967295u;
    printf("\nUINT32_MAX + 1 = %u\n", u + 1);
    return 0;
}
