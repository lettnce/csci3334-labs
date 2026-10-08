// Integer division by zero: the hardware decides what happens.
//
// On x86-64 the idiv instruction raises exception 0 (divide error). The kernel
// handles it by sending the process SIGFPE, and the shell prints
// "Floating point exception", although no floating point is involved.
//
// On arm64 (Apple Silicon) the sdiv instruction does not raise any exception.
// It returns 0, and the program keeps going.
//
// Either way, dividing by zero is undefined behavior in C (Lecture 8). The C
// standard promises nothing, so neither result is "the right one".

#include <stdio.h>

int main(void) {
    volatile int zero = 0;      // volatile: the compiler must really divide at run time
    int x = 42;

    printf("about to compute 42 / 0\n");
    fflush(stdout);             // make sure the line is out before we (maybe) die
    int q = x / zero;
    printf("42 / 0 = %d, and the program is still running\n", q);
    return 0;
}
