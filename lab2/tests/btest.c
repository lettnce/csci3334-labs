/*
 * btest — Part 1 checker for Lab 2.
 *
 *   make test          check the warmup and all five
 *   make test P=0      check only the warmup, mystery0
 *   make test P=3      check only mystery3
 *
 * Your function is run side by side with the real one from the .o over a wide
 * range of inputs, including the awkward ones. A mismatch prints the input, so
 * you can go back to the disassembly knowing exactly which case you missed.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

/* The compiled originals, linked in from mystery*.o */
int mystery0(int, int);
int mystery1(int);
int mystery2(unsigned);
int mystery3(const int *, int);
int mystery4(int, int);
int mystery5(const char *);

/* Yours, from solutions.c */
int my_mystery0(int, int);
int my_mystery1(int);
int my_mystery2(unsigned);
int my_mystery3(const int *, int);
int my_mystery4(int, int);
int my_mystery5(const char *);

static int failures, checked, reported;

static void fail(const char *fn, const char *args, long long got, long long want)
{
    failures++;
    if (reported++) return;
    printf("  \033[31mFAIL\033[0m  %s %s\n", fn, args);
    printf("          yours returned %lld, the real one returned %lld\n", got, want);
}

static unsigned seed = 0x2468ACEu;
static unsigned rnd(void) { seed = seed * 1103515245u + 12345u; return seed; }

int main(int argc, char **argv)
{
    /* -1 means "all": P=0 has to select the warmup, not everything. */
    int only = (argc > 1 && argv[1][0]) ? atoi(argv[1]) : -1;
    int score = 0, bonus = 0;     /* mystery1-4: 15 each; mystery5: +10 */
    char buf[64];

    printf("\nPart 1 — Reverse Engineering\n");

    /* ---- mystery0 — warmup, not scored ------------------------------- */
    if (only < 0 || only == 0) {
        reported = 0;
        for (int a = -20; a <= 20; a++)
            for (int b = -20; b <= 20; b++) {
                long long g = my_mystery0(a, b), w = mystery0(a, b);
                checked++;
                if (g != w) { snprintf(buf, sizeof buf, "(%d, %d)", a, b);
                              fail("mystery0", buf, g, w); }
            }
        /* Kept small enough that the arithmetic cannot overflow. */
        for (int i = 0; i < 2000; i++) {
            int a = (int)(rnd() % 200000) - 100000;
            int b = (int)(rnd() % 200000) - 100000;
            long long g = my_mystery0(a, b), w = mystery0(a, b);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(%d, %d)", a, b);
                          fail("mystery0", buf, g, w); }
        }
        if (!reported) printf("  \033[32mPASS\033[0m  mystery0   (warmup, not scored)\n");
    }

    /* ---- mystery1 ---------------------------------------------------- */
    if (only < 0 || only == 1) {
        reported = 0;
        for (int n = -50; n <= 200; n++) {
            long long g = my_mystery1(n), w = mystery1(n);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(%d)", n);
                          fail("mystery1", buf, g, w); }
        }
        for (int i = 0; i < 2000; i++) {
            int n = (int)(rnd() % 4000) - 2000;
            long long g = my_mystery1(n), w = mystery1(n);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(%d)", n);
                          fail("mystery1", buf, g, w); }
        }
        if (!reported) { printf("  \033[32mPASS\033[0m  mystery1\n"); score += 15; }
    }

    /* ---- mystery2 ---------------------------------------------------- */
    if (only < 0 || only == 2) {
        reported = 0;
        static const unsigned edge[] = {0u, 1u, 2u, 3u, 0x80000000u, 0xFFFFFFFFu,
                                        0xAAAAAAAAu, 0x55555555u, 0x0F0F0F0Fu};
        for (size_t i = 0; i < sizeof edge / sizeof edge[0]; i++) {
            long long g = my_mystery2(edge[i]), w = mystery2(edge[i]);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(0x%08x)", edge[i]);
                          fail("mystery2", buf, g, w); }
        }
        for (int i = 0; i < 20000; i++) {
            unsigned x = rnd();
            long long g = my_mystery2(x), w = mystery2(x);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(0x%08x)", x);
                          fail("mystery2", buf, g, w); }
        }
        if (!reported) { printf("  \033[32mPASS\033[0m  mystery2\n"); score += 15; }
    }

    /* ---- mystery3 ---------------------------------------------------- */
    if (only < 0 || only == 3) {
        reported = 0;
        int a[64];

        /* n <= 0 has its own answer; check it before anything else. */
        for (int n = -3; n <= 0; n++) {
            long long g = my_mystery3(a, n), w = mystery3(a, n);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(a, %d)", n);
                          fail("mystery3", buf, g, w); }
        }
        /* All-equal arrays pin down the tie-breaking rule. */
        for (int i = 0; i < 64; i++) a[i] = 7;
        for (int n = 1; n <= 64; n++) {
            long long g = my_mystery3(a, n), w = mystery3(a, n);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(all-equal, %d)", n);
                          fail("mystery3", buf, g, w); }
        }
        for (int t = 0; t < 4000; t++) {
            int n = (int)(rnd() % 64) + 1;
            for (int i = 0; i < n; i++) a[i] = (int)rnd() % 200 - 100;
            long long g = my_mystery3(a, n), w = mystery3(a, n);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(random array, %d)", n);
                          fail("mystery3", buf, g, w); }
        }
        if (!reported) { printf("  \033[32mPASS\033[0m  mystery3\n"); score += 15; }
    }

    /* ---- mystery4 ---------------------------------------------------- */
    if (only < 0 || only == 4) {
        reported = 0;
        for (int a = -40; a <= 40; a++)
            for (int b = -40; b <= 40; b++) {
                long long g = my_mystery4(a, b), w = mystery4(a, b);
                checked++;
                if (g != w) { snprintf(buf, sizeof buf, "(%d, %d)", a, b);
                              fail("mystery4", buf, g, w); }
            }
        for (int i = 0; i < 8000; i++) {
            int a = (int)(rnd() % 100000) - 50000;
            int b = (int)(rnd() % 100000) - 50000;
            long long g = my_mystery4(a, b), w = mystery4(a, b);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(%d, %d)", a, b);
                          fail("mystery4", buf, g, w); }
        }
        if (!reported) { printf("  \033[32mPASS\033[0m  mystery4\n"); score += 15; }
    }

    /* ---- mystery5 ---------------------------------------------------- */
    if (only < 0 || only == 5) {
        reported = 0;
        static const char *fixed[] = {
            "", "abc", "123", "a1b2c3", "0000000000", "9", "/0", ":9",
            "no digits here!", "  42  ", "\x2f\x30\x39\x3a",
        };
        for (size_t i = 0; i < sizeof fixed / sizeof fixed[0]; i++) {
            long long g = my_mystery5(fixed[i]), w = mystery5(fixed[i]);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(\"%s\")", fixed[i]);
                          fail("mystery5", buf, g, w); }
        }
        /* NULL is handled specially — the guard before the loop. */
        {
            long long g = my_mystery5(NULL), w = mystery5(NULL);
            checked++;
            if (g != w) fail("mystery5", "(NULL)", g, w);
        }
        for (int t = 0; t < 4000; t++) {
            char s[33];
            int n = (int)(rnd() % 32);
            for (int i = 0; i < n; i++) s[i] = (char)(0x20 + rnd() % 0x5F);
            s[n] = '\0';
            long long g = my_mystery5(s), w = mystery5(s);
            checked++;
            if (g != w) { snprintf(buf, sizeof buf, "(\"%s\")", s);
                          fail("mystery5", buf, g, w); }
        }
        if (!reported) { printf("  \033[32mPASS\033[0m  mystery5   (bonus)\n"); bonus += 10; }
    }

    printf("\n-------------------------------------------\n");
    printf("Part 1 score: %d/60  +%d bonus   (%d checks, %d failed)\n",
           score, bonus, checked, failures);
    printf("-------------------------------------------\n");
    return failures != 0;
}
