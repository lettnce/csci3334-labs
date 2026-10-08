/* Lab 2 Extra Practice: your answers.
 *
 * Each function below matches one problem in lab2-asm.pdf. Replace the
 * body with C that behaves exactly like that problem's assembly, then run
 *
 *     make test F=<name>      e.g.  make test F=add3
 *
 * Until you write one, it returns 0 and its test fails.
 */

#include <stddef.h>

struct node { long val; struct node *next; };   /* for list_sum */

/* Easy 1: Three-Way Add */
long my_add3(long a, long b, long c) {
    /* your code */
    return 0;
}

/* Easy 2: A Scaled Offset */
int my_affine(int x) {
    /* your code */
    return 0;
}

/* Easy 3: Multiply Without a Multiply */
int my_times12(int x) {
    /* your code */
    return 0;
}

/* Easy 4: Offsets Are Bytes */
long my_first_plus_third(const long *p) {
    /* your code */
    return 0;
}

/* Easy 5: Two Loads, Two Stores */
void my_swap(long *a, long *b) {
    /* your code */
}

/* Easy 6: Mixed Widths */
int my_byte_sum(unsigned char a, signed char b) {
    /* your code */
    return 0;
}

/* Medium 1: Pick the Larger */
int my_max2(int a, int b) {
    /* your code */
    return 0;
}

/* Medium 2: Signed or Unsigned? */
int my_is_below(unsigned a, unsigned b) {
    /* your code */
    return 0;
}

/* Medium 3: Compute Both, Keep One */
int my_abs_diff(int a, int b) {
    /* your code */
    return 0;
}

/* Medium 4: A Counted Loop */
int my_power(int base, int exp) {
    /* your code */
    return 0;
}

/* Medium 5: Search */
int my_find(const int *a, int n, int key) {
    /* your code */
    return 0;
}

/* Medium 6: The Counter Counts Bytes */
int my_dot(const int *a, const int *b, int n) {
    /* your code */
    return 0;
}

/* Hard 1: Division That Rounds Toward Zero */
int my_div8(int x) {
    /* your code */
    return 0;
}

/* Hard 2: A Row of Five */
int my_grid_get(int g[][5], long i, long j) {
    /* your code */
    return 0;
}

/* Hard 3: Following Pointers */
long my_list_sum(const struct node *p) {
    /* your code */
    return 0;
}

/* Hard 4: Walking Two Strings */
int my_str_eq(const char *a, const char *b) {
    /* your code */
    return 0;
}

/* Hard 5: A Jump Table */
int my_apply(int op, int x) {
    /* your code */
    return 0;
}

/* Hard 6: Recursion and Saved Registers */
/* Recursive: call my_fib here, not fib (fib is the packet's assembly). */
int my_fib(int n) {
    /* your code */
    return 0;
}
