/* Lecture 5b: 0.1 is not 0.1, and what that costs you.
 *
 * One tenth has no finite binary expansion, exactly as one third has no finite
 * decimal expansion. What gets stored is the nearest representable value, and
 * every arithmetic step afterwards works on that near-miss instead.
 */
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
    /* Same digits, three different numbers. Widening the type moves the
       error; it never removes it. Note which type each line is: a float and
       a double round 0.1 to different places, so their printouts differ in
       the ninth decimal, not the seventeenth. */
    printf("0.1f as float    %.20f   0x%08X\n", 0.1f, bits_of(0.1f));
    printf("0.1  as double   %.20f\n", 0.1);
    printf("0.2  as double   %.20f\n", 0.2);
    printf("0.3  as double   %.20f\n", 0.3);
    printf("0.1 + 0.2        %.20f\n\n", 0.1 + 0.2);

    printf("0.1 + 0.2 == 0.3            -> %s\n",
           0.1 + 0.2 == 0.3 ? "true" : "false");
    printf("fabs((0.1+0.2) - 0.3) < 1e-9 -> %s\n\n",
           fabs((0.1 + 0.2) - 0.3) < 1e-9 ? "true" : "false");

    /* Ten steps of 0.1 do not land on 1.0, so `!=` would loop forever.
       Count the steps instead of testing the accumulated value. */
    double sum = 0.0;
    for (int i = 0; i < 10; i++)
        sum += 0.1;
    printf("0.1 added ten times = %.20f\n", sum);
    printf("that == 1.0 -> %s\n", sum == 1.0 ? "true" : "false");
    return 0;
}
