/* Lecture 5: why quantized inference accumulates in a wider type.
 *
 * An int8 weight times an int8 activation is at most 127*127 = 16129, which
 * needs 15 bits. Add up 1024 of them and you need 25. Keep the running total
 * in int8, or even int16, and it wraps long before the layer is finished.
 */
#include <stdint.h>
#include <stdio.h>

#define N 1024

int main(void)
{
    int8_t w[N], a[N];
    for (int i = 0; i < N; i++) {
        w[i] = 100;
        a[i] = 100;
    }

    int16_t narrow = 0;
    int32_t wide   = 0;
    for (int i = 0; i < N; i++) {
        narrow = (int16_t)(narrow + (int16_t)w[i] * a[i]);   /* wraps */
        wide  += (int32_t)w[i] * a[i];                       /* fits  */
    }

    long long worst = (long long)N * 127 * 127;
    int bits = 1;                          /* the sign bit */
    for (long long v = worst; v; v >>= 1)
        bits++;

    printf("one product 127*127     %d   fits in int16\n", 127 * 127);
    printf("%d of them, worst case  %lld   needs %d bits with the sign\n\n",
           N, worst, bits);
    printf("accumulated in int16_t  %d   <- wrapped\n", narrow);
    printf("accumulated in int32_t  %d\n", wide);
    printf("\nINT16_MAX is %d; this run's true total is %lld\n",
           INT16_MAX, (long long)N * 100 * 100);
    printf("This is why every int8 kernel keeps an int32 accumulator.\n");
    return 0;
}
