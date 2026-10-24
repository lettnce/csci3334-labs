/*
 * memlib.c - the simulated heap for Lab 3. Do not change this file.
 *
 * One MAX_HEAP-byte region is allocated when the driver starts. mem_sbrk()
 * moves a break pointer through it, so the heap is always one contiguous run
 * of bytes from mem_heap_lo() to mem_heap_hi().
 *
 * Bytes past the break are filled with 0xA5. Memory the kernel hands to a
 * real allocator is not filled with anything useful either, so an allocator
 * that reads past the end of its heap gets garbage here instead of a
 * convenient zero, and the bug shows up in the tests.
 */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memlib.h"

#define POISON 0xA5

static char *heap_lo;   /* first byte of the region */
static char *brk_ptr;   /* one past the last byte of the heap */

void mem_init(void)
{
    void *p;
    if (posix_memalign(&p, 4096, MAX_HEAP) != 0) {
        fprintf(stderr, "mem_init: could not get %zu bytes for the heap\n",
                (size_t)MAX_HEAP);
        exit(2);
    }
    heap_lo = p;
    brk_ptr = heap_lo;
    memset(heap_lo, POISON, MAX_HEAP);
}

void mem_deinit(void)
{
    free(heap_lo);
    heap_lo = brk_ptr = NULL;
}

void mem_reset_brk(void)
{
    memset(heap_lo, POISON, (size_t)(brk_ptr - heap_lo));
    brk_ptr = heap_lo;
}

void *mem_sbrk(intptr_t incr)
{
    if (incr < 0) {
        fprintf(stderr, "mem_sbrk: incr is negative (%ld); this heap never shrinks\n",
                (long)incr);
        errno = EINVAL;
        return (void *)-1;
    }
    if ((size_t)incr > MAX_HEAP - (size_t)(brk_ptr - heap_lo)) {
        errno = ENOMEM;
        return (void *)-1;
    }
    char *old = brk_ptr;
    brk_ptr += incr;
    return old;
}

void *mem_heap_lo(void)   { return heap_lo; }
void *mem_heap_hi(void)   { return brk_ptr - 1; }
size_t mem_heapsize(void) { return (size_t)(brk_ptr - heap_lo); }
