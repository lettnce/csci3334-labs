// Which system call does malloc use? Run under strace on Linux:
//
//   strace -e trace=brk,mmap,munmap ./02-malloc-brk-mmap
//
// glibc's malloc serves small requests from the sbrk heap (the brk system
// call) and large ones, 128 KiB and up by default, with a separate mmap.
// The lines this program prints mark each step in strace's output.

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Storing each pointer here makes the program really use the memory, so the
// compiler cannot delete the malloc and free calls as useless.
static void *volatile keep;

static void say(const char *s) {
    if (write(1, s, strlen(s)) < 0)     // write, not printf: printf would
        exit(1);                        // call malloc itself
}

int main(void) {
    say("malloc 100 KiB\n");
    char *small = malloc(100 * 1024);
    keep = small;
    say("malloc 200 KiB\n");
    char *large = malloc(200 * 1024);
    keep = large;
    say("free 200 KiB\n");
    free(large);                        // an mmap'd block goes back at once
    say("free 100 KiB\n");
    free(small);                        // a heap block stays in the heap
    return 0;
}
