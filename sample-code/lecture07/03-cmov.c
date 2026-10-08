// When the compiler branches, and when it refuses to.
//
//     make asm   ->  compare the four functions
//
// A mispredicted branch costs ~15-20 cycles, so where it can, the compiler
// computes BOTH sides and picks one with `cmov` -- no jump, nothing to
// mispredict.
//
// The thing students expect and get wrong: the shape of your SOURCE does not
// decide this. `if/else` and `?:` below compile to byte-identical code. What
// decides it is whether evaluating both sides is SAFE.

#include <stdio.h>

// Both of these become: cmpl %esi, %edi / cmovll %edi, %eax
int as_if(int x, int y) {
    if (x < y) return x;
    else       return y;
}

int as_ternary(int x, int y) {
    return x < y ? x : y;
}

// Cannot be cmov: evaluating both sides means dereferencing p, and p may be
// NULL. The compiler MUST emit a real branch to avoid the fault.
int guarded(const int *p) {
    return p ? *p : 0;
}

// Cannot be cmov either: one side has a side effect, and cmov cannot un-do
// a store.
int counted(int x, int *calls) {
    if (x < 0) { (*calls)++; return -x; }
    return x;
}

int main(void) {
    printf("as_if(3, 7)      = %d\n", as_if(3, 7));
    printf("as_ternary(3, 7) = %d   <- same instructions as as_if\n",
           as_ternary(3, 7));

    int v = 99, calls = 0;
    printf("guarded(&v)      = %d\n", guarded(&v));
    printf("guarded(NULL)    = %d   <- had to branch: *p would fault\n",
           guarded(NULL));
    printf("counted(-5)      = %d   <- had to branch: it has a side effect\n",
           counted(-5, &calls));

    puts("\nThe first two are the same machine code. The last two are not,");
    puts("and neither is a choice you made in the source.");
    return 0;
}
