/* Lecture 5: shifting is not dividing, once the value goes negative. */
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    printf(" 7 / 2  = %2d     7 >> 1 = %2d   agree\n", 7 / 2, 7 >> 1);
    printf("-7 / 2  = %2d    -7 >> 1 = %2d   they do not\n", -7 / 2, -7 >> 1);
    printf("-8 / 2  = %2d    -8 >> 1 = %2d   exact, so they agree again\n\n",
           -8 / 2, -8 >> 1);

    /* / rounds toward zero; an arithmetic >> rounds down. Bias the value
       first and the shift matches the division. This is exactly what the
       compiler emits when you write x / 2. */
    int x = -7, k = 1;
    printf("(x + (1 << k) - 1) >> k  = %d   <- the bias makes it match\n\n",
           (x + (1 << k) - 1) >> k);

    /* % takes the sign of the left operand, which is not what "mod" means. */
    printf("-5 / 2 = %d,  -5 %% 2 = %d\n", -5 / 2, -5 % 2);
    printf("check:  (-5/2)*2 + (-5%%2) = %d\n\n", (-5 / 2) * 2 + (-5 % 2));

    /* Shift counts must be less than the width, and the value should be
       unsigned. 1 << 31 on a 32-bit int is undefined; 1U << 31 is not. */
    uint32_t high = UINT32_C(1) << 31;
    printf("1U << 31        0x%08X\n", high);
    printf("0xFFFFFFFF >> 4 0x%08X   <- unsigned: zeros shift in\n",
           0xFFFFFFFFu >> 4);
    printf("-8 >> 2         %d          <- signed: the sign bit shifts in\n",
           -8 >> 2);
    return 0;
}
