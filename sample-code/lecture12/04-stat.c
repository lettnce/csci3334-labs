// Print a file's metadata with stat. No read() call: the bytes are never
// touched.
//
//   ./04-stat 04-stat.c
//   ./04-stat .

#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s path\n", argv[0]); return 1; }

    struct stat st;
    if (stat(argv[1], &st) < 0) { perror(argv[1]); return 1; }

    printf("size:   %lld bytes\n", (long long)st.st_size);
    printf("blocks: %lld (512-byte units)\n", (long long)st.st_blocks);
    printf("inode:  %llu\n", (unsigned long long)st.st_ino);
    printf("mode:   %o (permissions %o)\n", (unsigned)st.st_mode,
           (unsigned)(st.st_mode & 0777));
    if (S_ISREG(st.st_mode)) printf("type:   regular file\n");
    if (S_ISDIR(st.st_mode)) printf("type:   directory\n");
    return 0;
}
