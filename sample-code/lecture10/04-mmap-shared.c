// A shared anonymous mapping made before fork is a channel between parent and
// child: both see the same physical page.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    char *shm = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shm == MAP_FAILED) { perror("mmap"); exit(1); }

    pid_t pid = fork();
    if (pid == 0) {
        strcpy(shm, "hello from child");  // child writes
        _exit(0);
    }
    wait(NULL);
    printf("Got: %s\n", shm);             // parent reads the same page
    munmap(shm, 4096);
    return 0;
}
