// What happens when a user program runs an instruction reserved for the kernel.
//
// Part 1 reads the CPU's current privilege level. On x86-64 it is the low two
// bits of the %cs register: 0 means kernel mode, 3 means user mode.
//
// Part 2 runs hlt ("halt the CPU until the next interrupt"). Only kernel mode
// may do that. The CPU refuses, raises a general protection fault
// (exception 13), and the kernel ends the program with SIGSEGV. The shell
// reports it as "Segmentation fault".
//
// Linux x86-64 only (inline x86 assembly):   make linux

#include <stdio.h>

int main(void) {
    unsigned short cs;
    __asm__("movw %%cs, %0" : "=r"(cs));
    printf("privilege level: %d  (3 = user mode, 0 = kernel mode)\n", cs & 3);
    fflush(stdout);

    printf("running hlt...\n");
    fflush(stdout);
    __asm__ volatile("hlt");
    printf("still running\n");      // never printed
    return 0;
}
