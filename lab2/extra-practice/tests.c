/* Lab 2 Extra Practice: test your C against the packet's assembly.
 *
 * listings.s holds every Part One listing from lab2-asm.pdf, exactly as
 * printed, under the plain names (add3, fib, ...). Your versions in mine.c are
 * named my_add3, my_fib, ... Each pair is run on the same inputs, so a function
 * passes only if it behaves exactly like the assembly you read.
 *
 *     make test           test all eighteen
 *     make test F=fib     test one
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

struct node { long val; struct node *next; };

#define PAIR(ret, name, ...) ret name(__VA_ARGS__); ret my_##name(__VA_ARGS__);
PAIR(long, add3, long, long, long)
PAIR(int, affine, int)
PAIR(int, times12, int)
PAIR(long, first_plus_third, const long *)
PAIR(void, swap, long *, long *)
PAIR(int, byte_sum, unsigned char, signed char)
PAIR(int, max2, int, int)
PAIR(int, is_below, unsigned, unsigned)
PAIR(int, abs_diff, int, int)
PAIR(int, power, int, int)
PAIR(int, find, const int *, int, int)
PAIR(int, dot, const int *, const int *, int)
PAIR(int, div8, int)
PAIR(int, grid_get, int (*)[5], long, long)
PAIR(long, list_sum, const struct node *)
PAIR(int, str_eq, const char *, const char *)
PAIR(int, apply, int, int)
PAIR(int, fib, int)

/* Results per function, in packet order. */
static const char *NAMES[] = {"add3", "affine", "times12", "first_plus_third",
    "swap", "byte_sum", "max2", "is_below", "abs_diff", "power", "find", "dot",
    "div8", "grid_get", "list_sum", "str_eq", "apply", "fib"};
#define NFUN (int)(sizeof NAMES / sizeof NAMES[0])
static int checks[NFUN], fails[NFUN];
static const char *only;          /* test just this one, or all when NULL */

static int slot(const char *n) {
    for (int i = 0; i < NFUN; i++)
        if (strcmp(NAMES[i], n) == 0) return i;
    return 0;
}
#define WANT(n) (!only || strcmp(only, n) == 0)

static void chk(const char *n, long long got, long long want, const char *how) {
    int i = slot(n);
    checks[i]++;
    if (got != want && fails[i]++ < 3)
        printf("  FAIL %-16s %s : your C gives %lld, the assembly gives %lld\n",
               n, how, got, want);
}
/* Only calls the functions when this one is being tested, so an unfinished
 * function elsewhere in mine.c cannot crash the test you asked for. */
#define CHK(n, g, w, fmt, ...) do { if (WANT(n)) { char b_[96];                \
        snprintf(b_, sizeof b_, fmt, __VA_ARGS__);                             \
        chk(n, (long long)(g), (long long)(w), b_); } } while (0)

static unsigned rnd(void) { return ((unsigned)rand() << 17) ^ ((unsigned)rand() << 8) ^ (unsigned)rand(); }
static int ri(void) { return (int)rnd(); }

static const int EDGE[] = {0, 1, -1, 2, -2, 7, -7, 8, -8, 9, -9, 100, -100,
                           INT_MAX, INT_MIN, INT_MAX - 1, INT_MIN + 1};
#define NEDGE (int)(sizeof EDGE / sizeof EDGE[0])

int main(int argc, char **argv)
{
    if (argc > 1 && argv[1][0]) {
        only = argv[1];
        int known = 0;
        for (int i = 0; i < NFUN; i++) known |= strcmp(NAMES[i], only) == 0;
        if (!known) {
            printf("no function called %s. Try one of:", only);
            for (int i = 0; i < NFUN; i++) printf(" %s", NAMES[i]);
            printf("\n");
            return 2;
        }
    }
    srand(3334);

    for (int i = 0; i < 20000; i++) {
        long a = (long)rnd() << 32 | rnd(), b = (long)rnd() << 32 | rnd(), c = ri();
        CHK("add3", my_add3(a, b, c), add3(a, b, c), "(%ld, %ld, %ld)", a, b, c);
    }

    for (int k = 0; k < NEDGE + 20000; k++) {
        int x = k < NEDGE ? EDGE[k] : ri();
        CHK("affine", my_affine(x), affine(x), "(%d)", x);
        CHK("times12", my_times12(x), times12(x), "(%d)", x);
        CHK("div8", my_div8(x), div8(x), "(%d)", x);
    }

    for (int t = 0; t < 2000; t++) {
        long p[3] = {(long)rnd() << 32 | rnd(), ri(), (long)rnd() << 32 | rnd()};
        CHK("first_plus_third", my_first_plus_third(p), first_plus_third(p), "{%ld, _, %ld}", p[0], p[2]);

        if (WANT("swap")) {
            long a1 = ri(), b1 = ri(), a2 = a1, b2 = b1;
            my_swap(&a1, &b1);
            swap(&a2, &b2);
            CHK("swap", a1, a2, "(*a = %ld)", a2);
            CHK("swap", b1, b2, "(*b = %ld)", b2);
        }
    }

    for (int a = 0; a < 256; a++)
        for (int b = 0; b < 256; b++)
            CHK("byte_sum", my_byte_sum((unsigned char)a, (signed char)b),
                byte_sum((unsigned char)a, (signed char)b), "(%d, %d)", a, (signed char)b);

    for (int i = 0; i < NEDGE; i++)
        for (int j = 0; j < NEDGE; j++) {
            int a = EDGE[i], b = EDGE[j];
            CHK("max2", my_max2(a, b), max2(a, b), "(%d, %d)", a, b);
            CHK("abs_diff", my_abs_diff(a, b), abs_diff(a, b), "(%d, %d)", a, b);
            CHK("is_below", my_is_below((unsigned)a, (unsigned)b), is_below((unsigned)a, (unsigned)b),
                "(%u, %u)", (unsigned)a, (unsigned)b);
        }
    for (int t = 0; t < 20000; t++) {
        int a = ri(), b = ri();
        CHK("max2", my_max2(a, b), max2(a, b), "(%d, %d)", a, b);
        CHK("abs_diff", my_abs_diff(a, b), abs_diff(a, b), "(%d, %d)", a, b);
        CHK("is_below", my_is_below((unsigned)a, (unsigned)b), is_below((unsigned)a, (unsigned)b),
            "(%u, %u)", (unsigned)a, (unsigned)b);
    }

    for (int base = -6; base <= 6; base++)
        for (int e = -3; e <= 40; e++)
            CHK("power", my_power(base, e), power(base, e), "(%d, %d)", base, e);

    for (int t = 0; t < 4000; t++) {
        int a[32], b[32], n = (int)(rnd() % 36) - 3;
        for (int i = 0; i < 32; i++) { a[i] = (int)(rnd() % 21) - 10; b[i] = ri(); }
        int key = (int)(rnd() % 21) - 10;
        int m = n < 32 ? n : 32;
        CHK("find", my_find(a, m, key), find(a, m, key), "(a, %d, %d)", m, key);
        CHK("dot", my_dot(a, b, m), dot(a, b, m), "(a, b, %d)", m);
    }

    {
        int g[4][5];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 5; j++) g[i][j] = ri();
        for (long i = 0; i < 4; i++)
            for (long j = 0; j < 5; j++)
                CHK("grid_get", my_grid_get(g, i, j), grid_get(g, i, j), "(g, %ld, %ld)", i, j);
    }

    for (int len = 0; len <= 12; len++) {
        struct node nodes[12];
        for (int i = 0; i < len; i++) {
            nodes[i].val = (long)ri() * 1000;
            nodes[i].next = i + 1 < len ? &nodes[i + 1] : NULL;
        }
        const struct node *head = len ? &nodes[0] : NULL;
        CHK("list_sum", my_list_sum(head), list_sum(head), "(list of %d)", len);
    }

    {
        static const char *s[] = {"", "a", "b", "ab", "abc", "abd", "abcd", "ABC",
                                  "hello", "hell", "hello!", "\x7f", "\xff", "\xfe"};
        int ns = (int)(sizeof s / sizeof s[0]);
        for (int i = 0; i < ns; i++)
            for (int j = 0; j < ns; j++)
                CHK("str_eq", my_str_eq(s[i], s[j]), str_eq(s[i], s[j]),
                    "(\"%s\", \"%s\")", s[i], s[j]);
    }

    for (int op = -3; op <= 9; op++)
        for (int k = 0; k < NEDGE + 200; k++) {
            int x = k < NEDGE ? EDGE[k] : ri();
            CHK("apply", my_apply(op, x), apply(op, x), "(%d, %d)", op, x);
        }

    for (int n = -3; n <= 27; n++)
        CHK("fib", my_fib(n), fib(n), "(%d)", n);

    int passed = 0, tested = 0;
    printf("\n");
    for (int i = 0; i < NFUN; i++) {
        if (!checks[i]) continue;
        tested++;
        passed += !fails[i];
        if (fails[i])
            printf("  %-16s FAIL  (%d of %d inputs differ)\n", NAMES[i], fails[i], checks[i]);
        else
            printf("  %-16s PASS  (%d inputs)\n", NAMES[i], checks[i]);
    }
    printf("\n  %d of %d passed\n", passed, tested);
    return passed != tested;
}
