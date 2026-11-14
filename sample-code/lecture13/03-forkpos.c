// One open() before fork(): parent and child share one open file table
// entry, so they share one file position. The parent waits for the child,
// so the child always reads first.
//
//   ./03-forkpos

#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int out = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (write(out, "0123456789abcdefghij", 20) != 20) { perror("write"); return 1; }
    close(out);

    int fd = open("data.txt", O_RDONLY);      // one open, before fork
    if (fd < 0) { perror("data.txt"); return 1; }

    char buf[11] = {0};
    pid_t pid = fork();
    if (pid == 0) {                           // child
        if (read(fd, buf, 10) != 10) { perror("read"); return 1; }
        printf("child  read %s, position now %lld\n",
               buf, (long long)lseek(fd, 0, SEEK_CUR));
        return 0;
    }
    waitpid(pid, NULL, 0);                    // let the child go first
    if (read(fd, buf, 10) != 10) { perror("read"); return 1; }
    printf("parent read %s, position now %lld\n",
           buf, (long long)lseek(fd, 0, SEEK_CUR));
    close(fd);
    return 0;
}
