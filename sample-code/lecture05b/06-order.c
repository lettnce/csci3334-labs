/* Lecture 5b: floating-point addition is not associative.
 *
 * Every operation rounds its result to the nearest representable value. Change
 * which operations happen, or the order they happen in, and the roundings land
 * differently — so the answer changes. This is why two runs of the same
 * numerical program on different hardware need not agree bit for bit.
 */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    /* Three numbers, two groupings, two answers. The parentheses decide
       which intermediate result gets rounded, and that changes the total. */
    printf("(0.1 + 0.2) + 0.3 = %.20f\n", (0.1 + 0.2) + 0.3);
    printf("0.1 + (0.2 + 0.3) = %.20f\n", 0.1 + (0.2 + 0.3));
    printf("equal? %s\n\n", (0.1 + 0.2) + 0.3 == 0.1 + (0.2 + 0.3) ? "yes" : "no");

    /* A big enough number swallows a small one whole: the exact sum has to be
       rounded back to a float, and the nearest float is the big one again. */
    float big = 1.0e8f, small = 1.0f;
    printf("1e8f + 1.0f == 1e8f -> %s\n\n",
           big + small == big ? "true" : "false");

    /* One million small values added to one large one, two ways round. */
    enum { N = 1000001 };
    float *a = malloc(N * sizeof *a);
    if (!a)
        return 1;
    a[0] = 1.0e8f;
    for (int i = 1; i < N; i++)
        a[i] = 1.0f;

    float fwd = 0.0f;
    for (int i = 0; i < N; i++)          /* big first: each 1 gets rounded off */
        fwd += a[i];

    float rev = 0.0f;
    for (int i = N - 1; i >= 0; i--)     /* small first: they accumulate       */
        rev += a[i];

    printf("forwards  %.1f\n", (double)fwd);
    printf("backwards %.1f\n", (double)rev);
    printf("exact     %.1f\n", 1.0e8 + (N - 1));
    printf("\nsame array, same additions, %s\n",
           fwd == rev ? "same answer" : "different answers");

    free(a);
    return 0;
}
