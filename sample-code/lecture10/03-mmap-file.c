// Map a file and read its bytes through a pointer. No read() call.
//
//   ./03-mmap-file 03-mmap-file.c

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s file\n", argv[0]); return 1; }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) { perror(argv[1]); return 1; }
    struct stat st;
    fstat(fd, &st);
    if (st.st_size == 0) { fprintf(stderr, "empty file\n"); return 1; }

    char *data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) { perror("mmap"); return 1; }

    printf("%s: %lld bytes mapped at %p\n", argv[1], (long long)st.st_size, (void *)data);
    printf("first line: ");
    for (off_t i = 0; i < st.st_size && data[i] != '\n'; i++)
        putchar(data[i]);                 // the file is just an array
    putchar('\n');

    munmap(data, st.st_size);
    close(fd);
    return 0;
}
