// List a directory the way the kernel stores it: one entry per name, each
// holding an inode number. The order is the directory's own order, not
// sorted, which is why ls sorts.
//
//   ./01-myls
//   ./01-myls /

#include <dirent.h>
#include <stdio.h>

int main(int argc, char **argv) {
    const char *path = argc == 2 ? argv[1] : ".";

    DIR *d = opendir(path);
    if (d == NULL) { perror(path); return 1; }

    struct dirent *e;
    while ((e = readdir(d)) != NULL)          // NULL: no more entries
        printf("%10llu  %s\n", (unsigned long long)e->d_ino, e->d_name);

    closedir(d);
    return 0;
}
