// Copy-on-write, seen from the program. After fork, parent and child use the
// same virtual address. The child writes, and only the child's copy changes.
// Counting the child's page faults shows the kernel copying pages on write.
//
//   ./05-cow                (fault counts are exact on Linux)

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#define PAGES 1000
#define PAGE  4096

static long minor_faults(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_minflt;
}

int x = 1;

int main(void) {
    char *buf = malloc(PAGES * PAGE);
    for (int i = 0; i < PAGES; i++)
        buf[i * PAGE] = 'p';              // parent owns 1000 real pages

    pid_t pid = fork();
    if (pid == 0) {
        x = 2;                            // child writes: its own copy
        long before = minor_faults();
        for (int i = 0; i < PAGES; i++)
            buf[i * PAGE] = 'c';          // each write copies one page
        long faults = minor_faults() - before;
        printf("child:  &x = %p, x = %d\n", (void *)&x, x);
        printf("child:  wrote %d shared pages, %ld page faults\n", PAGES, faults);
        fflush(stdout);                   // _exit skips stdio's buffers (Lecture 9)
        _exit(0);
    }
    wait(NULL);
    printf("parent: &x = %p, x = %d\n", (void *)&x, x);
    printf("parent: buf[0] = '%c' (the child's writes did not reach it)\n", buf[0]);
    free(buf);
    return 0;
}
