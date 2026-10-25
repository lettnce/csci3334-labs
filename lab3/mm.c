/*
 * mm.c - Lab 3: a memory allocator.
 *
 * Name: Aron Ledesma
 *
 * Design (Level 5: replace this paragraph with a few sentences of your own):
 * how a free block is found, when a block is split, when blocks are
 * coalesced, and, if you did Level 4, how your free list works.
 *
 * Rules:
 *   - All memory comes from mem_sbrk(). Do not call malloc, calloc, realloc,
 *     free, sbrk or mmap, and do not declare global or static arrays.
 *   - Keep the block layout below. The driver's heap checker tests read it.
 *   - Every payload address must be a multiple of 16.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "memlib.h"
#include "mm.h"

/* ====================================================================
 * Block layout (given, do not change)
 *
 * The heap is a fence followed by blocks, one after another:
 *
 *   heap+0  heap+8
 *   +-------+---------+---------+-----+---------+
 *   | fence | block   | block   | ... | block   |
 *   +-------+---------+---------+-----+---------+
 *           ^ heap_start                        ^ heap_end
 *
 * One block:
 *
 *   +----------+----------------------------------+----------+
 *   |  Header  |  payload: what mm_malloc returns  |  Footer  |
 *   |  8 bytes |  (a multiple of 16 as an address) |  8 bytes |
 *   +----------+----------------------------------+----------+
 *   ^ h        ^ payload_of(h)                    ^ footer_of(h)
 *   |<-------------------- h->size --------------------------->|
 *
 * The footer is a copy of the header. It is the "boundary tag" from
 * Lecture 11: it lets the block AFTER this one find this one's start.
 *
 * The fence is a footer that reads as "allocated, size 0". It sits just
 * before the first block, so checking the previous block's footer is safe
 * even for the first block: it says "not free".
 * ==================================================================== */

typedef struct header {
    uint32_t size;     /* the whole block in bytes: header + payload + footer.
                          Always a multiple of 16, and at least MIN_BLOCK. */
    uint32_t is_free;  /* 1 if the block is free, 0 if it is allocated */
} Header;

typedef Header Footer;

#define ALIGNMENT 16
#define OVERHEAD  (sizeof(Header) + sizeof(Footer))   /* 16 bytes per block */
#define MIN_BLOCK 32       /* header + 16 bytes of payload + footer */

static Header *heap_start;  /* the first block */
static char   *heap_end;    /* one past the last byte of the last block */

/* ---- Helpers (given). Each is one line of pointer arithmetic. -------- */

/* "unused" only stops the compiler warning about a helper you have not
 * used yet. */
#define HELPER static inline __attribute__((unused))

/* n rounded up to a multiple of 16 */
HELPER size_t round_up(size_t n)
{
    return (n + ALIGNMENT - 1) & ~(size_t)(ALIGNMENT - 1);
}
/* the payload of block h, and back again */
HELPER void   *payload_of(Header *h)  { return h + 1; }
HELPER Header *header_of(void *p)     { return (Header *)p - 1; }
/* block h's footer: its last 8 bytes */
HELPER Footer *footer_of(Header *h)   { return (Footer *)((char *)h + h->size) - 1; }
/* the block right after h. For the last block this is heap_end: check at_end() */
HELPER Header *next_block(Header *h)  { return (Header *)((char *)h + h->size); }
/* the footer of the block right before h (the fence, for the first block) */
HELPER Footer *prev_footer(Header *h) { return (Footer *)h - 1; }
/* the block right before h. Only use it when prev_footer(h)->is_free */
HELPER Header *prev_block(Header *h)  { return (Header *)((char *)h - prev_footer(h)->size); }
/* is h at (or past) the end of the heap, so not a block at all? */
HELPER int     at_end(Header *h)      { return (char *)h >= heap_end; }

/* Write block h's header AND footer in one go. Use this every time a block
 * changes size or changes between free and allocated. */
HELPER void set_block(Header *h, size_t size, int is_free)
{
    h->size = (uint32_t)size;
    h->is_free = (uint32_t)is_free;
    *footer_of(h) = *h;
}

/* ==================================================================== */
/* Level 1: mm_malloc and mm_free with an implicit free list            */

/*
 * The block size for a request of n bytes: room for the header, the payload
 * and the footer, rounded up to a multiple of 16, and never less than
 * MIN_BLOCK. For example, a request of 48 bytes needs a 64-byte block.
 */
static size_t block_size(size_t n)
{
    n+=16;
    size_t size = round_up(n);
    return (n < MIN_BLOCK) ? MIN_BLOCK : size;
}

/*
 * First fit: walk the blocks from heap_start until the end of the heap and
 * return the first free block whose size is at least size. Return NULL if
 * there is none. This is the loop on the Lecture 11 slide "An Implicit Free
 * List".
 */
static Header *find_fit(size_t size)
{
    for (Header *h = heap_start; !at_end(h); h = next_block(h))
        if (h->is_free && h->size >= size)
            return h;
    return NULL;
}

/*
 * No free block fits: ask mem_sbrk for size more bytes, move heap_end, and
 * make the new space one FREE block of size bytes with set_block. Return it,
 * or NULL if mem_sbrk fails. mem_sbrk, like sbrk, fails with (void *)-1,
 * not NULL.
 */
static Header *grow_heap(size_t size)
{
    int *br = mem_sbrk(size);
    if (br == (void *)-1)
        return NULL;

    heap_end+=size;
    set_block((Header *)br, size, 1);
    return (Header *)br;
}

/*
 * Hand out free block h for a request that needs size bytes.
 *   Level 1: mark the whole block allocated (set_block, keeping h->size).
 *   Level 2: if the leftover, h->size - size, is at least MIN_BLOCK, split:
 *            h becomes an allocated block of size bytes and the rest becomes
 *            a new free block right after it.
 */
static void place(Header *h, size_t size)
{
    size_t old = h->size;
    if (old - size >= MIN_BLOCK) {
        set_block(h, size, 0);
        Header *next = next_block(h);
        set_block(next, old - size, 1);
    }
    else
        set_block(h, old, 0);
}
/* Start a new, empty heap: just the fence. Given. Called before each trace. */
int mm_init(void)
{
    Footer *fence = mem_sbrk(sizeof(Footer));
    if (fence == (void *)-1)
        return -1;
    fence->size = 0;
    fence->is_free = 0;          /* reads as "allocated" */
    heap_start = (Header *)(fence + 1);
    heap_end = (char *)heap_start;
    /* Level 4: reset your free list(s) here too. */
    return 0;
}

/* Given: the four steps. You write the functions it calls. */
void *mm_malloc(size_t n)
{
    if (n == 0 || n > MAX_HEAP)
        return NULL;
    size_t size = block_size(n);
    Header *h = find_fit(size);
    if (h == NULL)
        h = grow_heap(size);
    if (h == NULL)
        return NULL;
    place(h, size);
    return payload_of(h);
}

/* ==================================================================== */
/* Level 2: coalescing with boundary tags                               */

/*
 * Block h has just been freed. Merge it with the next block if that one is
 * free, and with the previous block if that one is free (use prev_footer to
 * find out). Return the merged block. Do not read past heap_end: the last
 * block has no next block.
 */
static Header *coalesce(Header *h)
{
    // merging blocks based on free conditions (each case is a line or two)

    // neither
    if (!prev_footer(h)->is_free && !next_block(h)->is_free) { return h; }
    // next only
    else if (!at_end(next_block(h)) && next_block(h)->is_free && !prev_footer(h)->is_free) {
        set_block(h, h->size + next_block(h)->size, 1);
        return h;
    }
    // prev only
    else if (!at_end(next_block(h)) && prev_footer(h)->is_free && !next_block(h)->is_free) {
        set_block(prev_block(h), prev_block(h)->size + h->size, 1);
        h = prev_block(h);
        return h;
    }
    // both
    else {
        // size_t next_size = next_block(h)->size;
        // set_block(prev_block(h), prev_block(h)->size + h->size, 1);
        // h = prev_block(h);
        // set_block(h, h->size + next_size, 1);
        return h;
    }
}

void mm_free(void *p)
{
    if (p == NULL)
        return;
    Header *h = header_of(p);
    /* YOUR CODE HERE (Level 1): mark h free. */
    set_block(h, h->size, 1);

    coalesce(h);   /* does nothing until you write coalesce at Level 2 */
}

/* ==================================================================== */
/* Level 3: realloc and calloc                                          */

/*
 * Resize the block at p to n bytes and return the new payload address.
 * The first min(old size, n) bytes must be the same afterwards.
 *   p == NULL: same as mm_malloc(n)
 *   n == 0:    same as mm_free(p), and return NULL
 */
void *mm_realloc(void *p, size_t n)
{
    /* YOUR CODE HERE */
    (void)p;
    (void)n;
    return NULL;
}

/*
 * nmemb * size bytes, all zero. Return NULL if nmemb * size does not fit in
 * a size_t: check that before you multiply.
 */
void *mm_calloc(size_t nmemb, size_t size)
{
    /* YOUR CODE HERE */
    (void)nmemb;
    (void)size;
    return NULL;
}

/* ==================================================================== */
/* Level 5: the heap checker                                            */

/*
 * Walk the whole heap and check that it is consistent. Return 0 if it is.
 * If it is not, print what is wrong to stderr and return nonzero.
 * `./mdriver -c` calls this after every request. The handout lists what to
 * check.
 */
int mm_check(void)
{
    /* YOUR CODE HERE */
    return 0;
}

/* Print every block. Given. `./mdriver -v -d trace` calls it after each
 * request; you can also call it yourself while debugging. */
void mm_print_heap(void)
{
    char *lo = mem_heap_lo();
    printf("      heap: %zu bytes\n", mem_heapsize());
    for (Header *h = heap_start; !at_end(h); h = next_block(h)) {
        printf("      heap+%-6ld %-5s %6u bytes   payload at heap+%ld\n",
               (long)((char *)h - lo), h->is_free ? "free" : "alloc", h->size,
               (long)((char *)payload_of(h) - lo));
        if (h->size < MIN_BLOCK || h->size % ALIGNMENT != 0) {
            printf("      (that size is impossible; stopping here)\n");
            break;
        }
    }
}
