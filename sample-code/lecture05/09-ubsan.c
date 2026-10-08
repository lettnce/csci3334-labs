/* Lecture 5: this program is deliberately broken. Build it twice.
 *
 *   gcc -std=c17 -O2 09-ubsan.c -o 09-ubsan
 *   ./09-ubsan 2147483647 1              prints something, says nothing
 *
 *   gcc -std=c17 -O2 -fsanitize=undefined 09-ubsan.c -o 09-ubsan
 *   ./09-ubsan 2147483647 1              tells you the file and line
 *
 * The sanitizer costs speed, so it is a debugging build, not a shipping one.
 * Turn it on for every lab you hand in and it will find the bug before the
 * grader does.
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: %s <x> <y>\n", argv[0]);
        return 1;
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    int sum = x + y;                 /* signed overflow is undefined */
    printf("%d + %d = %d\n", x, y, sum);
    return 0;
}
