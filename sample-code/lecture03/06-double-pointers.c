/* Lecture 3 — "Why char **argv" and "A Matrix, Two Ways".
 *
 * Run it with arguments to see argv:   ./06-double-pointers hello -n 5
 *
 * The matrix half is the point worth watching: the rows of an int ** are
 * separate allocations, so they come back in whatever order malloc chose.
 * A flat int[3][4] has its rows exactly cols*sizeof(int) bytes apart, always.
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    printf("argc = %d\n", argc);
    for (int i = 0; i < argc; i++)
        printf("  argv[%d] = %p -> \"%s\"\n", i, (void *)argv[i], argv[i]);
    printf("  argv[%d] = %p   <- the NULL terminator\n", argc, (void *)argv[argc]);

    /* a matrix as an array of row pointers */
    int rows = 3, cols = 4;
    int **m = malloc(rows * sizeof *m);
    for (int i = 0; i < rows; i++) {
        m[i] = malloc(cols * sizeof *m[i]);
        for (int j = 0; j < cols; j++) m[i][j] = i * cols + j;
    }
    printf("\nm[1][2] = %d\n", m[1][2]);
    for (int i = 0; i < rows; i++)
        printf("  row %d at %p\n", i, (void *)m[i]);

    /* Gaps between consecutive rows. malloc MAY hand back adjacent blocks, and
     * often does for small ones — so a single run can look contiguous. It is
     * not promised, and the third row usually gives the game away. Run it a few
     * times: a flat int[3][4] is always exactly cols*sizeof(int) apart. */
    for (int i = 1; i < rows; i++)
        printf("  row%d - row%d = %ld bytes\n", i, i - 1,
               (long)((char *)m[i] - (char *)m[i - 1]));
    printf("  a flat int[3][4] would be %zu, every time\n", cols * sizeof(int));
    for (int i = 0; i < rows; i++) free(m[i]);
    free(m);
    return 0;
}
