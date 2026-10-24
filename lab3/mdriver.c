/*
 * mdriver.c - the Lab 3 test driver. Do not change this file.
 *
 * Runs one trace of malloc/free/realloc/calloc requests against your mm.c
 * and checks every single one:
 *
 *   - the payload address is a multiple of 16
 *   - the whole payload is inside the heap
 *   - the payload does not overlap any other block that is still allocated
 *   - the data the driver wrote into a block is still there when the block
 *     is freed or reallocated (so your allocator never wrote over a payload)
 *   - realloc keeps the old data, calloc returns zeros
 *
 * Usage: ./mdriver [options] <trace>
 *
 *   -v        print every request and the address it got
 *   -d        print the heap (mm_print_heap) after each request with -v,
 *             or once at the end without it
 *   -c        call mm_check() after every request; it must return 0
 *   -u PCT    fail unless utilization is at least PCT percent
 *   -t SEC    fail unless the trace runs in at most SEC seconds of CPU time
 *   -C N      heap checker test N (1-3): break the heap on purpose and
 *             expect mm_check() to notice
 *
 * Trace format, one request per line ('#' starts a comment):
 *
 *   a <id> <size>             p[id] = mm_malloc(size)
 *   f <id>                    mm_free(p[id])
 *   r <id> <size>             p[id] = mm_realloc(p[id], size)
 *   c <id> <nmemb> <size>     p[id] = mm_calloc(nmemb, size)
 *   h <bytes>                 check: the heap is at most this many bytes now.
 *                             If not, the comment just above it is printed.
 *
 * Utilization is the most payload bytes that were allocated at any one time
 * (the peak), divided by the size of the heap at the end. An allocator that
 * never reuses anything scores low; one that wastes nothing scores 100%.
 */
#define _XOPEN_SOURCE 700
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

#define WATCHDOG_SECONDS 60   /* give up on a trace after this long */
#define GRANULE 16            /* payloads are 16-byte aligned */

typedef struct {
    char   kind;        /* a f r c h */
    int    id;
    size_t size;        /* a, r, c: bytes; h: heap limit */
    size_t nmemb;       /* c only */
    int    line;        /* line number in the trace file */
    char  *note;        /* h only: the comment above it */
} Op;

typedef struct {
    unsigned char *p;   /* NULL when not allocated */
    size_t size;
    unsigned seed;      /* what the payload was filled with */
} Block;

static const char *trace_name;
static Op    *ops;
static int    nops;
static int    nids;
static Block *blocks;
static uint32_t *owner;        /* per 16-byte granule of the heap: id + 1, or 0 */
static unsigned next_seed = 1;
static size_t live_bytes, peak_bytes;
static int opt_verbose, opt_dump, opt_check;

static volatile sig_atomic_t cur_op = -1;  /* index into ops[]; -1 = in mm_init,
                                              -2 = in a heap checker test,
                                              -3 = in the warm-up */
static volatile sig_atomic_t in_check;     /* 1 while inside mm_check() */
static volatile sig_atomic_t timing;       /* 1 during the timing run */
static int allocations;                    /* blocks handed out in this run */

/* ------------------------------------------------------------------------ */
/* Reporting                                                                */

static void op_text(int i, char *buf, size_t n)
{
    const Op *o = &ops[i];
    switch (o->kind) {
    case 'a': snprintf(buf, n, "a %d %zu", o->id, o->size); break;
    case 'f': snprintf(buf, n, "f %d", o->id); break;
    case 'r': snprintf(buf, n, "r %d %zu", o->id, o->size); break;
    case 'c': snprintf(buf, n, "c %d %zu %zu", o->id, o->nmemb, o->size); break;
    case 'h': snprintf(buf, n, "h %zu", o->size); break;
    default:  snprintf(buf, n, "?"); break;
    }
}

static void where(char *buf, size_t n)
{
    char t[96];
    if (cur_op == -3) {
        snprintf(buf, n, "in the warm-up before the trace (mm_malloc of 100, 200 and "
                 "300 bytes, then mm_free of the second)");
    } else if (cur_op == -2) {
        snprintf(buf, n, "during the test%s", in_check ? ", inside mm_check" : "");
    } else if (cur_op < 0) {
        snprintf(buf, n, "in mm_init");
    } else {
        op_text(cur_op, t, sizeof t);
        snprintf(buf, n, "%sline %d (%s)%s", timing ? "timing run, " : "",
                 ops[cur_op].line, t, in_check ? ", inside mm_check" : "");
    }
}

static void fail(const char *fmt, ...)
{
    char w[160];
    va_list ap;
    where(w, sizeof w);
    printf("FAIL  %s, %s:\n      ", trace_name, w);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
    fflush(stdout);
    exit(1);
}

static void on_signal(int sig)
{
    char w[160], msg[600];
    const char *what =
        sig == SIGSEGV ? "a segmentation fault (a bad pointer)" :
        sig == SIGBUS  ? "a bus error (a bad or misaligned pointer)" :
        sig == SIGFPE  ? "an arithmetic error (division by zero?)" :
        sig == SIGABRT ? "abort() (a failed assert?)" :
        sig == SIGALRM ? "no result: still running after the time limit. "
                         "Is there an infinite loop, maybe a block of size 0?" :
                         "a signal";
    where(w, sizeof w);
    const char *first = cur_op >= 0 && allocations == 0 && !timing
        ? "      This was the first request of the trace. If you keep a free list,\n"
          "      check that mm_init resets it: the driver calls mm_init more than once.\n"
        : "";
    int n = snprintf(msg, sizeof msg, "FAIL  %s, %s:\n      crashed with %s\n%s",
                     trace_name ? trace_name : "?", w, what, first);
    if (sig == SIGALRM)
        n = snprintf(msg, sizeof msg, "FAIL  %s, %s:\n      %s\n",
                     trace_name ? trace_name : "?", w, what);
    if (n > 0) {
        if ((size_t)n > sizeof msg) n = sizeof msg;
        ssize_t r = write(STDOUT_FILENO, msg, (size_t)n);
        (void)r;
    }
    _exit(1);
}

static void install_handlers(void)
{
    static char altstack[1 << 16];
    stack_t ss = { .ss_sp = altstack, .ss_size = sizeof altstack, .ss_flags = 0 };
    sigaltstack(&ss, NULL);

    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = on_signal;
    sa.sa_flags = SA_ONSTACK;
    sigemptyset(&sa.sa_mask);
    int sigs[] = { SIGSEGV, SIGBUS, SIGFPE, SIGABRT, SIGILL, SIGALRM };
    for (size_t i = 0; i < sizeof sigs / sizeof sigs[0]; i++)
        sigaction(sigs[i], &sa, NULL);
}

static double cpu_seconds(void)
{
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + ru.ru_utime.tv_usec / 1e6 +
           (double)ru.ru_stime.tv_sec + ru.ru_stime.tv_usec / 1e6;
}

/* ------------------------------------------------------------------------ */
/* Reading a trace                                                          */

static void read_trace(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("FAIL  %s: cannot open it: %s\n", path, strerror(errno));
        exit(2);
    }
    char line[512], note[1024] = "";
    int cap = 1024, lineno = 0;
    ops = malloc(sizeof *ops * (size_t)cap);
    nops = 0;
    nids = 0;

    while (fgets(line, sizeof line, f)) {
        lineno++;
        char *s = line;
        while (isspace((unsigned char)*s)) s++;
        if (*s == '#') {
            /* Remember the comment block just above an 'h' line. */
            s++;
            while (*s == ' ') s++;
            s[strcspn(s, "\n")] = '\0';
            if (*s && strlen(note) + strlen(s) + 8 < sizeof note) {
                if (*note) strcat(note, "\n      ");
                strcat(note, s);
            }
            continue;
        }
        if (*s == '\0') { note[0] = '\0'; continue; }

        if (nops == cap) {
            cap *= 2;
            ops = realloc(ops, sizeof *ops * (size_t)cap);
        }
        Op *o = &ops[nops];
        memset(o, 0, sizeof *o);
        o->line = lineno;
        o->kind = *s;
        unsigned long long a = 0, b = 0, c = 0;
        int got = sscanf(s + 1, "%llu %llu %llu", &a, &b, &c);
        int ok = 0;
        switch (o->kind) {
        case 'a': case 'r': ok = got == 2; o->id = (int)a; o->size = (size_t)b; break;
        case 'f':           ok = got == 1; o->id = (int)a; break;
        case 'c':           ok = got == 3; o->id = (int)a; o->nmemb = (size_t)b;
                                           o->size = (size_t)c; break;
        case 'h':           ok = got == 1; o->id = 0; o->size = (size_t)a;
                            o->note = strdup(note); break;
        }
        if (!ok || (o->kind != 'h' && a > 10000000)) {
            printf("FAIL  %s: line %d is not a valid request: %s", path, lineno, line);
            exit(2);
        }
        if (o->kind != 'h' && o->id + 1 > nids) nids = o->id + 1;
        note[0] = '\0';
        nops++;
    }
    fclose(f);
    blocks = calloc((size_t)nids + 1, sizeof *blocks);
}

/* ------------------------------------------------------------------------ */
/* Checking blocks                                                          */

static unsigned char pattern(unsigned seed, size_t i)
{
    return (unsigned char)(seed * 167u + (unsigned)i * 13u + 7u);
}

static void fill(Block *b)
{
    b->seed = next_seed++;
    for (size_t i = 0; i < b->size; i++)
        b->p[i] = pattern(b->seed, i);
}

static ptrdiff_t off(const void *p)
{
    return (const char *)p - (const char *)mem_heap_lo();
}

/* The first n bytes of b must still hold what fill() wrote. */
static void verify(int id, const Block *b, const unsigned char *p, size_t n,
                   const char *when, const char *hint)
{
    for (size_t i = 0; i < n; i++) {
        if (p[i] != pattern(b->seed, i))
            fail("block %d's data changed %s:\n      byte %zu (at heap+%td) is 0x%02x, "
                 "but the driver wrote 0x%02x there.\n      %s",
                 id, when, i, off(p + i), p[i], pattern(b->seed, i), hint);
    }
}

static const char *OVERWRITTEN =
    "Something in mm.c wrote over a block that was still allocated.";

static void mark(int id, const Block *b, uint32_t val)
{
    size_t first = (size_t)off(b->p) / GRANULE;
    size_t last = ((size_t)off(b->p) + b->size - 1) / GRANULE;
    for (size_t g = first; g <= last; g++)
        owner[g] = val ? (uint32_t)id + 1 : 0;
}

/* p was just returned for block id with size bytes. Check it and record it. */
static void accept(int id, void *vp, size_t size, const char *fn)
{
    unsigned char *p = vp;
    char *lo = mem_heap_lo();
    char *end = (char *)mem_heap_hi() + 1;

    if (p == NULL)
        fail("%s returned NULL.\n      The heap is nowhere near full, so it "
             "should have returned a block.", fn);
    if ((uintptr_t)p % 16 != 0)
        fail("%s returned heap+%td, which is not a multiple of 16.\n"
             "      Payloads must be 16-byte aligned.", fn, off(p));
    if ((char *)p < lo || (char *)p > end || size > (size_t)(end - (char *)p))
        fail("%s returned a %zu-byte block at heap+%td, which is not inside "
             "the heap (heap+0 to heap+%td).%s", fn, size, off(p), end - lo,
             allocations == 0 ? "\n      This is the first block of the trace. mm_init "
                                "must reset every global, such as a free list."
                              : "");
    allocations++;

    size_t first = (size_t)off(p) / GRANULE;
    size_t last = ((size_t)off(p) + size - 1) / GRANULE;
    for (size_t g = first; g <= last; g++) {
        if (owner[g]) {
            int other = (int)owner[g] - 1;
            fail("%s returned heap+%td for %zu bytes, which overlaps block %d "
                 "(heap+%td, %zu bytes).\n      Block %d is still allocated.",
                 fn, off(p), size, other, off(blocks[other].p),
                 blocks[other].size, other);
        }
    }

    Block *b = &blocks[id];
    b->p = p;
    b->size = size;
    mark(id, b, 1);
    live_bytes += size;
    if (live_bytes > peak_bytes) peak_bytes = live_bytes;
}

static void release(int id)
{
    Block *b = &blocks[id];
    mark(id, b, 0);
    live_bytes -= b->size;
    b->p = NULL;
    b->size = 0;
}

/* ------------------------------------------------------------------------ */
/* Running a trace                                                          */

static void do_op(int i)
{
    Op *o = &ops[i];
    Block *b = &blocks[o->id];
    char t[96];

    switch (o->kind) {
    case 'a': {
        if (b->p) fail("the trace allocates block %d twice (a bug in the trace)", o->id);
        void *p = mm_malloc(o->size);
        if (o->size > MAX_HEAP) {
            if (p != NULL)
                fail("mm_malloc(%zu) returned a block, but the heap can never be "
                     "bigger than %zu bytes.\n      It must return NULL.",
                     o->size, (size_t)MAX_HEAP);
            break;
        }
        snprintf(t, sizeof t, "mm_malloc(%zu)", o->size);
        accept(o->id, p, o->size, t);
        fill(b);
        break;
    }
    case 'f':
        if (!b->p) fail("the trace frees block %d, which is not allocated", o->id);
        verify(o->id, b, b->p, b->size, "while it was allocated", OVERWRITTEN);
        {
            void *p = b->p;
            release(o->id);
            mm_free(p);
        }
        break;
    case 'r': {
        unsigned char *old = b->p;
        size_t oldsize = b->size;
        if (old) {
            verify(o->id, b, old, oldsize, "while it was allocated", OVERWRITTEN);
            release(o->id);
        }
        void *p = mm_realloc(old, o->size);
        if (o->size == 0) {
            if (p != NULL)
                fail("mm_realloc(p, 0) must free p and return NULL");
            break;
        }
        snprintf(t, sizeof t, "mm_realloc(%s, %zu)", old ? "p" : "NULL", o->size);
        accept(o->id, p, o->size, t);
        if (old) {
            /* b->seed still holds the old block's pattern */
            size_t keep = oldsize < o->size ? oldsize : o->size;
            verify(o->id, b, b->p, keep, "in mm_realloc",
                   "mm_realloc must copy the old data (the first min(old size, new size) bytes)\n"
                   "      into the block it returns.");
        }
        fill(b);
        break;
    }
    case 'c': {
        if (b->p) fail("the trace allocates block %d twice (a bug in the trace)", o->id);
        void *p = mm_calloc(o->nmemb, o->size);
        int overflow = o->nmemb != 0 && o->size > SIZE_MAX / o->nmemb;
        if (overflow || o->nmemb * o->size > MAX_HEAP) {
            if (p != NULL)
                fail("mm_calloc(%zu, %zu) returned a block, but %s.\n"
                     "      It must return NULL.", o->nmemb, o->size,
                     overflow ? "nmemb * size does not fit in a size_t"
                              : "the heap can never be that big");
            break;
        }
        snprintf(t, sizeof t, "mm_calloc(%zu, %zu)", o->nmemb, o->size);
        accept(o->id, p, o->nmemb * o->size, t);
        for (size_t k = 0; k < b->size; k++)
            if (b->p[k] != 0)
                fail("mm_calloc(%zu, %zu): byte %zu of the block (heap+%td) is "
                     "0x%02x, not 0", o->nmemb, o->size, k, off(b->p + k), b->p[k]);
        fill(b);
        break;
    }
    case 'h':
        if (mem_heapsize() > o->size)
            fail("the heap is %zu bytes here, but it should be at most %zu.\n"
                 "      %s", mem_heapsize(), o->size,
                 o->note && *o->note ? o->note : "");
        break;
    }
}

static void print_op(int i)
{
    char t[96];
    const Op *o = &ops[i];
    op_text(i, t, sizeof t);
    if (o->kind == 'f')
        printf("%5d  %s\n", o->line, t);
    else if (o->kind == 'h')
        printf("%5d  %-20s  heap is %zu bytes, limit %zu\n", o->line, t, mem_heapsize(), o->size);
    else if (blocks[o->id].p)
        printf("%5d  %-20s  -> heap+%td\n", o->line, t, off(blocks[o->id].p));
    else
        printf("%5d  %-20s  -> NULL\n", o->line, t);
}

/* A few requests before the real run, so that the real run's mm_init is not
 * the first one. A real program never calls mm_init twice, but the driver
 * does, and mm_init must reset every global (a free list head, say). */
static void warm_up(void)
{
    mem_reset_brk();
    cur_op = -1;
    if (mm_init() != 0)
        fail("mm_init returned nonzero");
    cur_op = -3;
    void *a = mm_malloc(100), *b = mm_malloc(200), *c = mm_malloc(300);
    mm_free(b);
    (void)a; (void)c;
}

static void check_run(void)
{
    warm_up();
    allocations = 0;
    mem_reset_brk();
    memset(owner, 0, sizeof *owner * (MAX_HEAP / GRANULE));
    live_bytes = peak_bytes = 0;
    cur_op = -1;
    if (mm_init() != 0)
        fail("mm_init returned nonzero");

    for (int i = 0; i < nops; i++) {
        cur_op = i;
        do_op(i);
        if (opt_check) {
            in_check = 1;
            int r = mm_check();
            in_check = 0;
            if (r != 0)
                fail("mm_check() returned %d after this request.\n"
                     "      Either the heap is broken or the checker is wrong.", r);
        }
        if (opt_verbose) {
            print_op(i);
            if (opt_dump) mm_print_heap();
        }
    }
    /* Everything still allocated must still hold its data. */
    for (int id = 0; id < nids; id++)
        if (blocks[id].p)
            verify(id, &blocks[id], blocks[id].p, blocks[id].size,
                   "by the end of the trace", OVERWRITTEN);
}

/* The same requests again, with no checking, to time the allocator alone. */
static double timing_run(double limit)
{
    void **p = calloc((size_t)nids + 1, sizeof *p);
    timing = 1;
    mem_reset_brk();
    cur_op = -1;
    double t0 = cpu_seconds();
    if (mm_init() != 0)
        fail("mm_init returned nonzero");
    for (int i = 0; i < nops; i++) {
        const Op *o = &ops[i];
        cur_op = i;
        switch (o->kind) {
        case 'a': p[o->id] = mm_malloc(o->size); break;
        case 'f': mm_free(p[o->id]); p[o->id] = NULL; break;
        case 'r': p[o->id] = mm_realloc(p[o->id], o->size); break;
        case 'c': p[o->id] = mm_calloc(o->nmemb, o->size); break;
        }
        /* Already over the limit: stop rather than make you wait. */
        if (i % 1024 == 0 && cpu_seconds() - t0 > limit)
            break;
    }
    double t = cpu_seconds() - t0;
    timing = 0;
    free(p);
    return t;
}

/* ------------------------------------------------------------------------ */
/* Heap checker tests: break a correct heap and expect mm_check to notice.  */

typedef struct { uint32_t size, is_free; } Tag;   /* the Header/Footer layout */

static Tag *hdr(void *payload) { return (Tag *)payload - 1; }
static Tag *ftr(void *payload)
{
    Tag *h = hdr(payload);
    return (Tag *)((char *)h + h->size) - 1;
}

static int checker_test(int n)
{
    static const char *what[] = {
        "",
        "block B's footer says 48 bytes, but its header says 32",
        "block B's size is 24, which is not a multiple of 16 (its header and footer agree)",
        ("blocks A and B are next to each other and both free (B was marked free "
         "behind the allocator's back, so it was never coalesced or put on a free list)"),
    };
    trace_name = "heap checker test";
    nops = 0;
    mem_reset_brk();
    if (mm_init() != 0) { printf("FAIL  checker test %d: mm_init failed\n", n); return 1; }

    cur_op = -2;
    void *a = mm_malloc(16), *b = mm_malloc(16), *c = mm_malloc(16), *d = mm_malloc(16);
    if (!a || !b || !c || !d) {
        printf("FAIL  checker test %d: mm_malloc(16) returned NULL\n", n);
        return 1;
    }
    in_check = 1;
    int r = mm_check();
    in_check = 0;
    if (r != 0) {
        printf("FAIL  checker test %d: mm_check() returned %d on a correct heap of "
               "four 32-byte blocks A B C D.\n      It must return 0 when nothing is wrong.\n",
               n, r);
        return 1;
    }
    if (hdr(b)->size != 32 || ftr(b)->size != 32) {
        printf("FAIL  checker test %d: mm_malloc(16) did not make a 32-byte block with "
               "the Header/Footer layout from mm.c,\n      so this test cannot run.\n", n);
        return 1;
    }

    switch (n) {
    case 1: ftr(b)->size = 48; break;
    case 2: {
        Tag *h = hdr(b);
        h->size = 24;
        *ftr(b) = *h;
        break;
    }
    case 3:
        mm_free(a);
        hdr(b)->is_free = 1;
        ftr(b)->is_free = 1;
        break;
    default:
        printf("no checker test %d (there are 3)\n", n);
        return 2;
    }

    in_check = 1;
    r = mm_check();
    in_check = 0;
    if (r == 0) {
        printf("FAIL  checker test %d: mm_check() returned 0, but %s.\n", n, what[n]);
        return 1;
    }
    printf("ok: mm_check() returned %d when %s\n", r, what[n]);
    return 0;
}

/* ------------------------------------------------------------------------ */

static void usage(void)
{
    printf("usage: ./mdriver [-v] [-d] [-c] [-u PCT] [-t SEC] <trace>\n"
           "       ./mdriver -C N     heap checker test N (1-3)\n");
    exit(2);
}

int main(int argc, char **argv)
{
    double min_util = -1, max_time = -1;
    int ctest = 0, opt;

    setvbuf(stdout, NULL, _IOLBF, 0);
    while ((opt = getopt(argc, argv, "vdcu:t:C:h")) != -1) {
        switch (opt) {
        case 'v': opt_verbose = 1; break;
        case 'd': opt_dump = 1; break;
        case 'c': opt_check = 1; break;
        case 'u': min_util = atof(optarg); break;
        case 't': max_time = atof(optarg); break;
        case 'C': ctest = atoi(optarg); break;
        default:  usage();
        }
    }

    install_handlers();
    mem_init();

    if (ctest) {
        alarm(WATCHDOG_SECONDS);
        int r = checker_test(ctest);
        mem_deinit();
        return r;
    }
    if (optind != argc - 1) usage();

    trace_name = argv[optind];
    read_trace(trace_name);
    owner = calloc(MAX_HEAP / GRANULE, sizeof *owner);
    if (!owner) { printf("out of memory\n"); return 2; }

    /* With a time limit, time the trace first, in a child process. If it is
     * too slow, say so at once instead of making you wait for the slow
     * allocator to finish the checked run too. If the child crashes, the
     * checked run below finds the same bug and explains it. */
    double secs = -1;
    if (max_time > 0) {
        int fd[2];
        if (pipe(fd) != 0) { perror("pipe"); return 2; }
        fflush(stdout);
        pid_t pid = fork();
        if (pid == 0) {
            close(fd[0]);
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull >= 0) dup2(devnull, STDOUT_FILENO);   /* quiet: the parent reports */
            alarm((unsigned)(10 * max_time) + 2);   /* an infinite loop never checks the clock */
            double t = timing_run(max_time);
            ssize_t w = write(fd[1], &t, sizeof t);
            _exit(w == (ssize_t)sizeof t ? 0 : 1);
        }
        close(fd[1]);
        double t;
        int status = 0;
        ssize_t got = read(fd[0], &t, sizeof t);
        close(fd[0]);
        waitpid(pid, &status, 0);
        if (got == (ssize_t)sizeof t && WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            secs = t;
            if (secs > max_time) {
                printf("FAIL  %s: too slow: the requests took more than %.2f s of CPU time.\n"
                       "      Walking past every allocated block on every request is too slow here.\n",
                       trace_name, max_time);
                return 1;
            }
        }
    }

    alarm(WATCHDOG_SECONDS);
    check_run();
    double util = mem_heapsize() ? (double)peak_bytes / (double)mem_heapsize() : 0;
    size_t heap = mem_heapsize();
    if (opt_dump && !opt_verbose) mm_print_heap();

    int nreq = 0;
    for (int i = 0; i < nops; i++)
        nreq += ops[i].kind != 'h';
    int bad = min_util >= 0 && util * 100 < min_util;
    printf("%s  %d requests, peak %zu bytes in use, heap %zu bytes, utilization %.1f%%",
           bad ? "FAIL" : "ok:", nreq, peak_bytes, heap, util * 100);
    if (min_util >= 0) printf(" (needs %.0f%%)", min_util);
    if (max_time > 0) printf(", %.3f s (limit %.2f s)", secs, max_time);
    printf("\n");
    if (bad)
        printf("      utilization is too low: the heap grew to %zu bytes to hold at "
               "most %zu bytes at once.\n", heap, peak_bytes);

    free(owner);
    free(blocks);
    for (int i = 0; i < nops; i++) free(ops[i].note);
    free(ops);
    mem_deinit();
    return bad;
}
