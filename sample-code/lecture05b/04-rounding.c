/* Lecture 5b: rounding, and the two directions a conversion can lose data. */
#include <math.h>
#include <stdio.h>

int main(void)
{
    /* Ties go to the even neighbour, so a long column of them does not drift. */
    printf("printf(\"%%.0f\")  %.0f %.0f %.0f %.0f %.0f\n", 0.5, 1.5, 2.5, 3.5, 4.5);
    printf("round()        %.0f %.0f %.0f %.0f %.0f  <- ties away from zero\n",
           round(0.5), round(1.5), round(2.5), round(3.5), round(4.5));

    /* A cast is not rounding at all: it chops toward zero. */
    printf("\n(int) 3.9  = %d      (int) -3.9 = %d   <- truncation, not rounding\n",
           (int)3.9, (int)-3.9);

    /* A float keeps 24 significant bits, so past 2^24 the integers thin out. */
    printf("\n(float)16777216 = %.0f\n", (double)(float)16777216);
    printf("(float)16777217 = %.0f  <- 2^24 + 1 is not a float\n",
           (double)(float)16777217);
    printf("(double)16777217 = %.0f  <- a double has room\n",
           (double)16777217);

    /* An int can always be widened to double; the trip back can overflow. */
    printf("\nint -> double -> int   %d\n", (int)(double)2147483647);
    printf("int -> float  -> int   loses the low bits long before that\n");
    return 0;
}
