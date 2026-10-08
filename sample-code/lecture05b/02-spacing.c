/* Lecture 5b: floats are not spread evenly along the number line.
 *
 * Between 1 and 2 there are 2^23 of them. Between 2 and 4 there are the same
 * 2^23, spread over twice the distance — so each step is twice as wide. Keep
 * doubling and the step eventually passes 1, which is where a float stops
 * being able to count.
 */
#include <float.h>
#include <math.h>
#include <stdio.h>

int main(void)
{
    printf("start   next float up   gap\n");
    for (float x = 1.0f; x <= 1024.0f; x *= 4.0f)
        printf("%7g %15.9g   %g\n", (double)x,
               (double)nextafterf(x, INFINITY),
               (double)(nextafterf(x, INFINITY) - x));

    printf("\n2^23 = %.0f  gap %g\n", 8388608.0,
           (double)(nextafterf(8388608.0f, INFINITY) - 8388608.0f));
    printf("2^24 = %.0f  gap %g\n", 16777216.0,
           (double)(nextafterf(16777216.0f, INFINITY) - 16777216.0f));

    /* Once the gap reaches 2, adding 1 rounds straight back to where it was. */
    float f = 16777216.0f;                 /* 2^24 */
    printf("\nf == %.0f, f + 1 == %.0f  ->  %s\n",
           (double)f, (double)(f + 1.0f),
           f + 1.0f == f ? "the same number" : "different");

    printf("\nFLT_EPSILON = %g   (the gap just above 1.0)\n", (double)FLT_EPSILON);
    printf("DBL_EPSILON = %g   (a double has 53 bits, not 24)\n", DBL_EPSILON);
    return 0;
}
