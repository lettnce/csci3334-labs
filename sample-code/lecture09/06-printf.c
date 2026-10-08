// The smallest printf program. Run it under strace to see which system calls
// one printf turns into, and how many the program makes before main runs.
//
//   strace ./06-printf
//   strace -c ./06-printf

#include <stdio.h>

int main(void) {
    printf("hello\n");
    return 0;
}
