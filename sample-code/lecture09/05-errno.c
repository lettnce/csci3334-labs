// How a failed system call reports the failure.
//
// The kernel returns a negative error number, such as -2. The C library's
// wrapper turns that into a return value of -1 and stores the 2 in `errno`.
// So the rule for every system call wrapper is: check for -1, then read errno.

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    int fd = open("/no/such/file", O_RDONLY);
    int saved = errno;          // read errno right away, before anything else can change it

    printf("open returned   %d\n", fd);
    printf("errno is        %d  (ENOENT is %d)\n", saved, ENOENT);
    printf("strerror says   %s\n", strerror(saved));

    if (fd == -1)
        perror("open /no/such/file");   // the usual one-line report
    else
        close(fd);
    return 0;
}
