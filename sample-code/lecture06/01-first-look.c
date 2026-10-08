// The four functions we read the assembly of in class.
//
// They are deliberately tiny: each one should compile to a handful of
// instructions, so the whole function fits on a slide.
//
//     make asm   ->  01-first-look.s     the x86-64 assembly
//     make dis   ->  01-first-look.dis   the same thing, with the bytes
//
// Build it native (`make`) and it still runs — the numbers below are just
// there to prove the functions do what their names say.

#include <stdio.h>

// rdi = a, rsi = b, result in rax.  One instruction, and it is not `add`.
int add(int a, int b) {
    return a + b;
}

// No `imul` here either: 3 is 1 + 2, which an address expression can do.
long triple(long x) {
    return x * 3;
}

// `arr`'s pointer value arrives in rdi; `i` arrives in rsi. The compiler uses
// `(%rdi,%rsi,8)`: compute rdi + rsi*8 as an address, then read the long in
// memory there. The parentheses request that read; they do not make rdi into
// an address — it already holds the pointer value.
long index_it(const long *arr, long i) {
    return arr[i];
}

// Reads memory once per element. This is the loop we walk line by line.
long sum(const long *arr, long n) {
    long total = 0;
    for (long i = 0; i < n; i++)
        total += arr[i];
    return total;
}

int main(void) {
    long a[] = {10, 20, 30, 40};

    printf("add(2, 3)        = %d\n", add(2, 3));
    printf("triple(7)        = %ld\n", triple(7));
    printf("index_it(a, 2)   = %ld\n", index_it(a, 2));
    printf("sum(a, 4)        = %ld\n", sum(a, 4));
    return 0;
}
