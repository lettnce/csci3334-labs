/* Lecture 5b: the values at the two ends of the exponent field. */
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint32_t bits_of(float f)
{
    uint32_t u;
    memcpy(&u, &f, sizeof u);
    return u;
}

int main(void)
{
    float inf  = 1.0f / 0.0f;
    float ninf = -1.0f / 0.0f;
    float nan  = 0.0f / 0.0f;

    printf("1.0f/0.0f   %8f  0x%08X\n", (double)inf,  bits_of(inf));
    printf("-1.0f/0.0f  %8f  0x%08X\n", (double)ninf, bits_of(ninf));
    printf("0.0f/0.0f   %8f  0x%08X\n", (double)nan,  bits_of(nan));
    printf("+0.0f       %8f  0x%08X\n", 0.0,  bits_of(0.0f));
    printf("-0.0f       %8f  0x%08X  <- a different bit pattern\n",
           -0.0, bits_of(-0.0f));

    printf("\n0.0f == -0.0f   %s   <- but they compare equal\n",
           0.0f == -0.0f ? "true" : "false");
    printf("nan == nan      %s   <- always. Use isnan().\n",
           nan == nan ? "true" : "false");
    printf("isnan(nan)      %s\n", isnan(nan) ? "true" : "false");

    /* Below FLT_MIN the leading 1 is dropped, buying a few more digits of
       range at the cost of precision. This is gradual underflow. */
    printf("\nFLT_MIN       %g  0x%08X  smallest normal\n",
           (double)FLT_MIN, bits_of(FLT_MIN));
    printf("FLT_MIN/2     %g  0x%08X  denormal: exponent field is 0\n",
           (double)(FLT_MIN / 2), bits_of(FLT_MIN / 2));
    printf("FLT_TRUE_MIN  %g  0x%08X  one single mantissa bit\n",
           (double)FLT_TRUE_MIN, bits_of(FLT_TRUE_MIN));
    return 0;
}
