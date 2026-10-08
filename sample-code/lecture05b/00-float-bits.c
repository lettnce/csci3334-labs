/* Lecture 5b: look at the thirty-two bits of a float.
 *
 * print_binary is the one from Lecture 4, unchanged. A float and a uint32_t
 * are the same thirty-two bits; memcpy is how you move between them without
 * breaking C's aliasing rules.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void print_binary(uint32_t x)
{
    for (int i = 31; i >= 0; i--) {
        putchar((x & (1u << i)) ? '1' : '0');
        if (i % 4 == 0 && i != 0)
            putchar(' ');
    }
}

static void show(const char *label, float f)
{
    uint32_t bits;
    memcpy(&bits, &f, sizeof bits);

    printf("%s\n", label);
    printf("  Hex:     %08X\n", bits);
    printf("  Binary:  ");
    print_binary(bits);
    /* %f promotes the float to double, so this prints the value the 32 bits
       actually stand for -- not the 0.1 you typed. */
    printf("\n  Stored:  %.20f\n\n", (double)f);
}

int main(void)
{
    show("float f = 3.0f;", 3.0f);
    show("float f = 0.1f;", 0.1f);
    return 0;
}
