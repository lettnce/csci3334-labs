// Practice 5: count a file's newlines through mmap, then find the mapping in
// /proc/self/maps (Linux only; on macOS the count still works).
//
//   ./07-newlines /etc/services

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    if (argc != 2) return 1;
    struct stat st;
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) { perror(argv[1]); return 1; }
    fstat(fd, &st);                         // st.st_size: file length
    char *p = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED) { perror("mmap"); return 1; }
    long lines = 0;
    for (off_t i = 0; i < st.st_size; i++)  // the file is an array
        if (p[i] == '\n') lines++;
    printf("%ld newlines\n", lines);
    char buf[512];
    FILE *maps = fopen("/proc/self/maps", "r");
    if (!maps) return 0;                    // no /proc on macOS
    while (fgets(buf, sizeof buf, maps))
        if (strstr(buf, argv[1])) fputs(buf, stdout);   // our file only
    return 0;
}
