/* Lecture 5b: a float and a uint32_t are the same thirty-two bits.
 *
 * memcpy is how you look at them. Do NOT write *(uint32_t *)&f — reading an
 * object through a pointer of an unrelated type breaks C's aliasing rules.
 * At -O2 the memcpy compiles down to the same single instruction anyway.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint32_t bits_of(float f)
{
    uint32_t u;
    memcpy(&u, &f, sizeof u);
    return u;
}

/* 0 10000000 10000000000000000000000 — spaced into its three fields */
static void print_bits(uint32_t u)
{
    for (int i = 31; i >= 0; i--) {
        putchar((u >> i) & 1 ? '1' : '0');
        if (i == 31 || i == 23)
            putchar(' ');
    }
}

static void show(float f)
{
    uint32_t u    = bits_of(f);
    uint32_t sign = u >> 31;              /* 1 bit                         */
    uint32_t exp  = (u >> 23) & 0xFF;     /* 8 bits, biased by 127         */
    uint32_t man  = u & 0x7FFFFFu;        /* 23 bits, leading 1 not stored */

    printf("%6g  0x%08X  ", (double)f, u);
    print_bits(u);

    if (exp == 0 && man == 0)
        printf("  %czero\n", sign ? '-' : '+');
    else if (exp == 0)                    /* denormal: no implied leading 1 */
        printf("  %c%-10.9g x 2^-126\n", sign ? '-' : '+', man / 8388608.0);
    else
        printf("  %c%-10.9g x 2^%d\n", sign ? '-' : '+',
               1.0 + man / 8388608.0, (int)exp - 127);
}

int main(void)
{
    float values[] = { 0.0f, -0.0f, 1.0f, -1.0f, 0.5f, 3.0f, -3.0f,
                       5.75f, -5.75f, 0.1f };

    for (size_t i = 0; i < sizeof values / sizeof values[0]; i++)
        show(values[i]);
    return 0;
}
