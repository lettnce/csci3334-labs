// Unit 1 ended with four shapes: an integer, a pointer, a struct and a float.
// A register is 8 bytes. Here is what each of those four looks like when it
// is sitting in one.
//
// Nothing here is x86-specific — it runs on whatever machine you are on.
// The point is that the bytes carry no label saying which shape they are.

#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Print any object of <= 8 bytes the way a register dump would show it.
static void as_register(const char *what, const void *p, size_t n) {
    uint64_t reg = 0;                  // a register is 8 bytes, zero-filled
    memcpy(&reg, p, n);
    printf("  %-22s %zu bytes   0x%016llX\n", what, n, (unsigned long long)reg);
}

struct point { int x, y; };            // 8 bytes: two ints side by side

int main(void) {
    int      i = 42;
    long     l = -1;
    int     *p = &i;
    float    f = 3.0f;
    double   d = 3.0;
    struct point pt = {1, 2};

    puts("The same 8-byte box, six different meanings:\n");
    as_register("int 42",            &i,  sizeof i);
    as_register("long -1",           &l,  sizeof l);
    as_register("int * (an address)", &p,  sizeof p);
    as_register("float 3.0f",        &f,  sizeof f);
    as_register("double 3.0",        &d,  sizeof d);
    as_register("struct point{1,2}", &pt, sizeof pt);

    puts("\nRead the struct: 0x...0000000200000001 is y then x, little-endian.");
    puts("Read the float:  0x0000000040400000 is the 0x40400000 from Lecture 5b.");
    puts("\nNothing in the bits says which is which. Only the instruction does.");
    return 0;
}
