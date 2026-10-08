// Reading the bits of a float: one way that is defined, one that is not.
//
// Lecture 5b used memcpy without saying why. This is why.

#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Defined. Every compiler at -O1 turns this into a single move.
static uint32_t bits_memcpy(float f) {
    uint32_t u;
    memcpy(&u, &f, sizeof u);
    return u;
}

// Defined in C (not in C++). Reading a union member other than the one last
// written reinterprets the bytes -- C99 TC3 onward says so explicitly.
static uint32_t bits_union(float f) {
    union { float f; uint32_t u; } v;
    v.f = f;
    return v.u;
}

// NOT defined: this reads a float object through a uint32_t pointer, which
// violates C's aliasing rules. It happens to work today. That is not the same
// as being correct, and -O2 is free to reorder around it.
//
//     uint32_t bad(float f) { return *(uint32_t *)&f; }
//
// It is left commented out on purpose -- see the slides.

int main(void) {
    float f = 3.0f;

    printf("f            = %g\n", f);
    printf("bits_memcpy  = 0x%08X\n", bits_memcpy(f));
    printf("bits_union   = 0x%08X\n", bits_union(f));

    puts("\nSame answer, both defined. The pointer cast would also print this");
    puts("today -- and is still the one the standard does not allow.");
    puts("0x40400000 is the 3.0f you decoded in Lecture 5b.");
    return 0;
}
