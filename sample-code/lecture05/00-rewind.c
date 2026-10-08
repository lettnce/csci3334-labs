/* Lecture 5 rewind: the same eight bits, read two ways.
 *
 * Lecture 2 called this "One Byte, Two Readings". Nothing in memory says
 * whether a byte is signed. The bits are the bits; the *type* decides how the
 * top bit is read — as another power of two, or as a minus sign.
 */
#include <stdint.h>
#include <stdio.h>

/* print_binary from Lecture 4, narrowed to one byte. */
static void print_binary8(uint8_t x)
{
    for (int i = 7; i >= 0; i--) {
        putchar((x & (1u << i)) ? '1' : '0');
        if (i == 4)
            putchar(' ');
    }
}

static void show(uint8_t bits)
{
    printf("  ");
    print_binary8(bits);
    /* Same eight bits both times. The only difference is what the top bit is
       worth: +128 to the unsigned reading, -128 to the signed one. */
    printf("   %5u   %5d\n", bits, (int8_t)bits);
}

int main(void)
{
    printf("  %-9s   %5s   %5s\n", "bits", "uint8", "int8");
    for (unsigned v = 0; v <= 2; v++)
        show((uint8_t)v);
    show(127);
    show(128);
    show(129);
    show(200);
    show(254);
    show(255);

    /* The odometer: one past the top rolls back to the bottom.
     *
     * These two lines are not the same story. Unsigned arithmetic is defined
     * to wrap, so the first is a promise. In the second the addition is fine
     * — int8_t promotes to int, and 127 + 1 is exactly 128 — and it is the
     * conversion back into one byte that changes the value. C calls that
     * implementation-defined, which is weaker than a promise and stronger
     * than the undefined behaviour we meet later today. */
    uint8_t top = 255;
    printf("\n  255 + 1 as uint8_t  ->  %4u    defined: unsigned wraps\n",
           (unsigned)(uint8_t)(top + 1));

    int8_t stop = 127;
    printf("  127 + 1 as int8_t   ->  %4d    the add was fine; the squeeze "
           "back was not\n", (int)(int8_t)(stop + 1));
    return 0;
}
