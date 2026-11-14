// A file whose last name is removed still exists while a process has it
// open. The kernel frees the inode and its blocks at the last close.
//
//   ./02-unlink

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    int fd = open("temp.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("temp.txt"); return 1; }
    if (write(fd, "still here\n", 11) != 11) { perror("write"); return 1; }

    struct stat st;
    fstat(fd, &st);
    printf("before unlink: link count %lld\n", (long long)st.st_nlink);

    unlink("temp.txt");                       // remove the only name
    if (access("temp.txt", F_OK) < 0)        // look the name up again
        printf("after unlink:  the name temp.txt is gone\n");

    fstat(fd, &st);
    printf("after unlink:  link count %lld, size %lld bytes\n",
           (long long)st.st_nlink, (long long)st.st_size);

    char buf[64];
    lseek(fd, 0, SEEK_SET);                   // back to the start
    ssize_t n = read(fd, buf, sizeof buf);
    if (n < 0) { perror("read"); return 1; }
    printf("read %zd bytes through the descriptor: ", n);
    fwrite(buf, 1, n, stdout);

    close(fd);                                // now the kernel frees it
    return 0;
}
