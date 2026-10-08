// Comparisons, and the one that goes wrong.
//
//     make asm   ->  read the cmp / j-instruction pairs
//
// The signed-vs-unsigned trap at the bottom is the same Lecture 5 bug, seen
// from the instruction side: the VALUES are identical, only the jump differs.

#include <stdio.h>

int min(int x, int y) {          // cmpl %esi, %edi  /  cmovll  -- signed
    return x < y ? x : y;
}

int min_u(unsigned x, unsigned y) {   // cmpl %esi, %edi  /  cmovbl  -- unsigned
    return x < y ? x : y;
}

int is_zero(long n) {            // testq %rdi, %rdi  /  sete
    return n == 0;
}

int main(void) {
    int  a = -1,  b = 1;
    unsigned ua = (unsigned)a, ub = (unsigned)b;

    printf("as signed:    a = %d,  b = %d\n", a, b);
    printf("  a < b       %s   (jl:  signed compare)\n", a < b ? "true" : "false");

    printf("\nsame bits, as unsigned:\n");
    printf("  ua = %u, ub = %u\n", ua, ub);
    printf("  ua < ub     %s  (jb: unsigned compare)\n", ua < ub ? "true" : "false");

    puts("\nThe bits never changed. Only which jump the compiler picked.");
    puts("cmp sets BOTH answers in the flags; the j-instruction chooses one.");
    return 0;
}
