/* Lecture 5b: BF16 is FP32 with the bottom sixteen bits thrown away.
 *
 * That is the whole format. Same sign bit, same eight exponent bits, seven
 * mantissa bits instead of twenty-three. Converting is a truncation, which is
 * why hardware can do it for free — and why BF16 keeps FP32's entire range
 * while FP16, with only five exponent bits, cannot.
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

static float from_bits(uint32_t u)
{
    float f;
    memcpy(&f, &u, sizeof f);
    return f;
}

/* Round-to-nearest-even, the same rule the hardware uses. */
static float to_bf16(float f)
{
    uint32_t u = bits_of(f);
    uint32_t round = 0x7FFF + ((u >> 16) & 1);
    return from_bits((u + round) & 0xFFFF0000u);
}

int main(void)
{
    float values[] = { 1.0f, 3.14159265f, 0.1f, 100.5f, 1.0e-8f, 3.0e38f };

    printf("%14s  %14s  %-8s  %s\n", "FP32", "BF16", "rel err", "kept bits");
    for (size_t i = 0; i < sizeof values / sizeof values[0]; i++) {
        float f = values[i], b = to_bf16(f);
        printf("%14.8g  %14.8g  %-8.2g  0x%08X\n",
               (double)f, (double)b,
               f == 0.0f ? 0.0 : (double)((b - f) / f), bits_of(b));
    }

    /* FP16's exponent field is five bits: anything below about 6e-8 is zero.
       BF16 keeps FP32's eight, so the same value survives. */
    float grad = 1.0e-8f;
    printf("\na gradient of %g\n", (double)grad);
    printf("  in BF16 -> %g   (exponent range unchanged)\n", (double)to_bf16(grad));
    printf("  in FP16 -> 0    (below FP16's smallest denormal, 6e-8)\n");
    return 0;
}
