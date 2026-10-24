// The MMU enforces the prot bits. Each test runs in a child process, so the
// crash is reported instead of ending this program.
//
//   ./06-protect

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

static void try_write(const char *name, int prot) {
    char *page = mmap(NULL, 4096, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) { perror("mmap"); return; }

    pid_t pid = fork();
    if (pid == 0) {
        page[0] = 'x';                    // the write under test
        _exit(0);
    }
    int status;
    waitpid(pid, &status, 0);
    if (WIFSIGNALED(status))
        printf("%-22s write killed the child: signal %d (%s)\n",
               name, WTERMSIG(status), strsignal(WTERMSIG(status)));
    else
        printf("%-22s write succeeded\n", name);
    munmap(page, 4096);
}

int main(void) {
    try_write("PROT_READ|PROT_WRITE", PROT_READ | PROT_WRITE);
    try_write("PROT_READ", PROT_READ);
    try_write("PROT_NONE", PROT_NONE);
    return 0;
}
