/* Lecture 4 warm-up: print a number in binary, using nothing but Lecture 2.
 *
 * There is no printf conversion for binary — no "%b" in C17. You write it
 * yourself, and the tool for it is the mask: a value with exactly one bit set,
 * walked from the top bit down to the bottom.
 */
#include <stdint.h>
#include <stdio.h>

void print_binary(uint32_t x)
{
    for (int i = 31; i >= 0; i--) {
        putchar((x & (1u << i)) ? '1' : '0');
        if (i % 4 == 0 && i != 0)
            putchar(' ');       /* one space per nibble = one hex digit */
    }
}

static void show(const char *label, uint32_t x)
{
    printf("%-15s %08X  ", label, x);
    print_binary(x);
    putchar('\n');
}

int main(void)
{
    show("5", 5);
    show("42 (0x2A)", 42);
    show("1u << 31", 1u << 31);
    show("0xFF00FF00", 0xFF00FF00u);

    /* Two's complement, seen rather than described. */
    show("-1", (uint32_t)-1);
    show("-5", (uint32_t)-5);
    show("INT32_MIN", (uint32_t)INT32_MIN);

    /* Sign extension: a negative char widens by copying its top bit left. */
    signed char c = -5;
    show("(char)-5 -> int", (uint32_t)(int32_t)c);

    return 0;
}
