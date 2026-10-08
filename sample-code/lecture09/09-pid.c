// Every process has a number, and so does the process that started it.
//
// Run it twice. The PID changes each time, because each run is a new process.
// The parent PID stays the same, because both runs were started by the same
// shell. Compare it with `echo $$`, which prints the shell's own PID.

#include <stdio.h>
#include <unistd.h>

int main(void) {
    printf("my PID:        %d\n", (int)getpid());
    printf("my parent PID: %d\n", (int)getppid());
    return 0;
}
