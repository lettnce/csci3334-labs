/*
 * Lab 4 — Cache Simulator
 *
 * Build up in levels. Each is tested on its own, and each is worth points on
 * its own, so partial work is partial credit:
 *
 *     make test        every level
 *     make test L=1    just level 1
 *
 *   Level 1 (60 pts)  direct-mapped (-E 1): correct hits and misses
 *   Level 2 (15 pts)  set-associative: correct on traces that never evict
 *   Level 3 (10 pts)  LRU eviction: matches the reference exactly
 *   Level 4 (5 pts)   verbose mode (-v)
 *   Level 5 (10 pts)  clean code, no leaks, compiles without warnings
 *
 * Level 1 on its own is a passing grade for this lab. Get `-E 1` completely
 * right before you think about associativity — every later level is built on
 * the address decomposition you write for level 1.
 *
 * The argument parsing, the trace reader and the output formatting are done
 * for you. What you write is the cache itself.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ---- cache geometry, filled in from the command line -------------------- */

static int s_bits = -1; /* number of set-index bits; there are 2^s sets     */
static int E_lines;     /* lines per set (the slides call this W, "ways")   */
static int b_bits = -1; /* number of block-offset bits; block is 2^b bytes  */
static int verbose;

/* ---- the counters the grader reads -------------------------------------- */

static long hits, misses, evictions;

/*
 * A cache line. `valid` says whether it holds anything at all; `tag` is the
 * high part of an address; `last_used` is yours to use for LRU in level 3.
 */
typedef struct {
    int valid;
    unsigned long tag;
    unsigned long last_used;
} Line;

static Line *cache;             /* 2^s * E lines. See cache_init(). */

/* ------------------------------------------------------------------------ *
 * LEVEL 1 (60 pts) — allocate the cache, then handle one access
 * ------------------------------------------------------------------------ */

/*
 * Allocate 2^s sets of E lines each, all invalid to start with.
 *
 * Store them as one flat array — set i, line j lives at cache[i * E + j].
 * A flat array is simpler to free than an array of arrays, and level 5 checks
 * for leaks.
 *
 * Hint: calloc zeroes for you, which makes every line invalid already.
 */
static void cache_init(void)
{
    /* YOUR CODE HERE */
}

/*
 * Simulate one access to `addr`.
 *
 * Set *did_miss to 1 if this access missed, *did_evict to 1 if it had to
 * throw a valid line out to make room. Level 4 prints those; levels 1-3 only
 * need the counters right.
 *
 * The shape of it:
 *   1. split the address into tag / set index / block offset
 *   2. search that set for a valid line with a matching tag -> hit
 *   3. otherwise it is a miss. If the set has a free line, fill it.
 *   4. if the set is full, evict one and count the eviction
 *
 * Address layout, with the block offset at the bottom. Lecture 15 calls the
 * set number the "index":
 *
 *     tag = addr >> (b + s)
 *     set = (addr >> b) & ((1 << s) - 1)
 *
 * Check yourself on the Lecture 15 example: with -s 7 -b 6 (8 KB,
 * direct-mapped, 64-byte lines), 0x9064 is tag 4, set 65, offset 36.
 * traces/t1-basic.tr starts with exactly that address.
 *
 * For level 1 (E == 1) each set holds exactly one line, so step 4 is just
 * "overwrite it". Do not use `int` for addresses — they are 64-bit.
 */
static void access_cache(unsigned long addr, int *did_miss, int *did_evict)
{
    /* YOUR CODE HERE */
    (void)addr;
    *did_miss = 0;
    *did_evict = 0;
}

/* ------------------------------------------------------------------------ *
 * LEVEL 2 (15 pts) — associativity
 *
 * Search every line of the set, and fill any free line before evicting.
 * Tested on traces whose working set fits in the cache, so no eviction policy
 * is involved yet and any correct search passes.
 *
 * LEVEL 3 (10 pts) — LRU
 *
 * When a full set has to evict, throw out the line that was used longest ago.
 * The usual trick is a counter that increments on every access: stamp
 * `last_used` on each hit and each fill, then evict the smallest stamp.
 *
 * Both levels are changes to access_cache() above, not new functions.
 * ------------------------------------------------------------------------ */

/*
 * Release everything cache_init() allocated. Level 5 runs valgrind.
 */
static void cache_free(void)
{
    /* YOUR CODE HERE */
    (void)cache;   /* delete this line once you actually use `cache` */
}

/* ------------------------------------------------------------------------ *
 * Provided — argument parsing, trace reading, output. No need to change this.
 * ------------------------------------------------------------------------ */

static void usage(const char *prog)
{
    printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", prog);
}

int main(int argc, char **argv)
{
    const char *trace = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
        case 's': s_bits = atoi(optarg); break;
        case 'E': E_lines = atoi(optarg); break;
        case 'b': b_bits = atoi(optarg); break;
        case 't': trace = optarg; break;
        case 'v': verbose = 1; break;
        case 'h': usage(argv[0]); return 0;
        default:  usage(argv[0]); return 1;
        }
    }

    /* -s 0 is allowed: one set holding every line, i.e. fully associative. */
    if (s_bits < 0 || E_lines <= 0 || b_bits < 0 || !trace) {
        fprintf(stderr, "%s: missing required argument\n", argv[0]);
        usage(argv[0]);
        return 1;
    }

    cache_init();

    FILE *f = fopen(trace, "r");
    if (!f) { perror(trace); return 1; }

    char line[256];
    while (fgets(line, sizeof line, f)) {
        char op;
        unsigned long addr;
        int size;

        if (line[0] == 'I')            /* instruction fetch — not simulated */
            continue;
        if (sscanf(line, " %c %lx,%d", &op, &addr, &size) < 2)
            continue;

        int miss1, evict1, miss2, evict2;

        access_cache(addr, &miss1, &evict1);
        if (verbose)
            printf("%c 0x%lx %s%s", op, addr,
                   miss1 ? "miss" : "hit", evict1 ? " eviction" : "");

        /* M is a load followed by a store to the same address. The store
           cannot miss — the load just brought the line in. */
        if (op == 'M') {
            access_cache(addr, &miss2, &evict2);
            if (verbose)
                printf(" %s%s", miss2 ? "miss" : "hit",
                       evict2 ? " eviction" : "");
        }

        if (verbose)
            putchar('\n');
    }

    fclose(f);
    cache_free();

    if (verbose)
        putchar('\n');
    printf("hits:%ld misses:%ld evictions:%ld\n", hits, misses, evictions);
    return 0;
}
