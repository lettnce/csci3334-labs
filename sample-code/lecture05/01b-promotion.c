/* Lecture 5: promote, THEN calculate. A live demo.
 *
 * The question this answers: in
 *
 *     uint8_t a = 200, b = 100;
 *     uint8_t  small = a + b;      // 44
 *     uint16_t big   = a + b;      // 300
 *
 * is the sum computed in 8 bits and then widened, or widened and then
 * computed? Neither operand survives as a uint8_t: both are promoted to int
 * before the + runs, so the sum is the int 300 on BOTH lines. The only thing
 * that differs is the assignment.
 *
 * Things to try in class. Change the type of a and b and re-run:
 *
 *   uint16_t  sizeof(a + b) is still 4, and the type is still int -- an int
 *             can represent every uint16_t value, so that is where it goes.
 *   uint32_t  sizeof is STILL 4, but the type becomes unsigned int. An int
 *             cannot represent every uint32_t value, so the promotion takes
 *             the other branch of C17 6.3.1.1p2.
 *
 * Then make `small` an int instead of a uint8_t, and see where 44 goes.
 * (It does not go anywhere. There was never a 44 until you stored it.)
 */
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    uint8_t a = 200, b = 100;

    puts("--- 1. the part nobody expects ---");
    if (a + b > 255)
        puts("  a + b > 255 is TRUE — the sum never wrapped");
    else
        puts("  a + b > 255 is false");
    printf("  printf(\"%%d\", a + b) gives %d, not 44\n\n", a + b);

    puts("--- 2. why: the operands are not bytes any more ---");
    printf("  sizeof a        %zu\n", sizeof a);
    printf("  sizeof b        %zu\n", sizeof b);
    printf("  sizeof (a + b)  %zu   <- an int, four bytes\n", sizeof(a + b));

    /* _Generic asks the compiler what type an expression has, at compile
       time. It never evaluates the expression -- it just reads its type. */
    printf("  a + b has type  %s\n\n",
           _Generic((a + b),
                    int:      "int",
                    unsigned: "unsigned int",
                    uint8_t:  "uint8_t",
                    uint16_t: "uint16_t",
                    default:  "something else"));

    puts("--- 3. so where does 44 come from? the assignment ---");
    uint8_t  small = a + b;
    uint16_t big   = a + b;
    printf("  uint8_t  small = a + b;   ->  %u    300 does not fit in one byte\n",
           small);
    printf("  uint16_t big   = a + b;   ->  %u   300 fits in two\n\n", big);

    puts("--- 4. the same sum, three destinations ---");
    printf("  (uint8_t)(a + b)   %u\n", (unsigned)(uint8_t)(a + b));
    printf("  (uint16_t)(a + b)  %u\n", (unsigned)(uint16_t)(a + b));
    printf("  (int)(a + b)       %d\n", a + b);
    puts("\n  Promote, then calculate. The box only matters when you store it.");
    return 0;
}
