// 1000 calls to printf. How many write system calls is that?
//
// It depends on where the output goes. The C library keeps a buffer and only
// calls write when the buffer is flushed:
//   - to a terminal, it flushes at every newline   (line buffered)
//   - to a file or pipe, it flushes when 4096 bytes have piled up (fully buffered)
//
//   strace -c -e trace=write ./07-buffering               (terminal)
//   strace -c -e trace=write ./07-buffering > /dev/null   (not a terminal)

#include <stdio.h>

int main(void) {
    for (int i = 0; i < 1000; i++)
        printf("line %d\n", i);
    return 0;
}
