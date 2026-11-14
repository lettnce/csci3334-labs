// sbrk moves the end of the heap (the "program break"). sbrk(0) reports where
// it is now; sbrk(n) moves it up by n bytes and returns the old end, which is
// where the new bytes start.
//
//   ./01-sbrk
//   strace -e trace=brk ./01-sbrk       (Linux: sbrk is a library function;
//                                         the system call underneath is brk)
//
// Every measurement is taken before the first printf, because printf calls
// malloc for its buffer, and malloc moves the break too.

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void) {
    char *before = sbrk(0);                 // where the heap ends now
    char *p = sbrk(4096);                   // grow it by 4096 bytes
    if (p == (void *)-1)                    // sbrk fails with -1, not NULL
        return 1;
    p[0] = 'x';                             // the new bytes are ours to use
    p[4095] = 'y';
    char *after = sbrk(0);
    sbrk(-4096);                            // give the top 4096 bytes back
    char *shrunk = sbrk(0);

    printf("break before:   %p\n", (void *)before);
    printf("sbrk(4096) ->   %p   the old break: the new bytes start here\n", (void *)p);
    printf("break after:    %p   %ld bytes higher\n", (void *)after, (long)(after - before));
    printf("after sbrk(-4096): %p   %s\n", (void *)shrunk,
           shrunk == before ? "back where it started"
                            : "did not move back (macOS did not shrink it)");
    return 0;
}
