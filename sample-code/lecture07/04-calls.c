// The return address is a value in memory, and you can print it.
//
// `call` pushed it; `ret` will pop it. Until then it just sits on the stack
// like any other 8 bytes -- which is exactly what Lecture 8 exploits.

#include <stdio.h>

static void inner(void) {
    // Where inner() will return to -- an address inside outer().
    printf("  inner returns to      %p\n", __builtin_return_address(0));
}

static void outer(void) {
    printf("  outer returns to      %p\n", __builtin_return_address(0));
    inner();
}

int main(void) {
    puts("Return addresses, innermost call last:\n");
    outer();

    int local;
    printf("\n  a local in main is at %p\n", (void *)&local);
    puts("\nThe return address lives on the same stack as your locals,");
    puts("a few bytes away. Nothing marks it as special.");
    return 0;
}
