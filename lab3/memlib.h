/*
 * memlib.h - the simulated heap for Lab 3. Do not change this file.
 *
 * The driver gets one big region of memory from the C library when it starts
 * and hands it to your allocator a piece at a time through mem_sbrk(), the
 * way the kernel hands a real allocator memory through sbrk(). Your mm.c
 * gets all of its memory from mem_sbrk() and from nowhere else.
 */
#ifndef MEMLIB_H
#define MEMLIB_H

#include <stddef.h>
#include <stdint.h>

/* The heap can never grow past this many bytes (64 MB). */
#define MAX_HEAP ((size_t)64 << 20)

/*
 * Grow the heap by incr bytes and return the address of the first new byte,
 * which is the old end of the heap. Like sbrk(), it returns (void *)-1, not
 * NULL, when it fails: when incr is negative (this heap never shrinks) or when
 * the heap would grow past MAX_HEAP.
 */
void *mem_sbrk(intptr_t incr);

void  *mem_heap_lo(void);    /* address of the first byte of the heap */
void  *mem_heap_hi(void);    /* address of the LAST byte of the heap (lo - 1 when empty) */
size_t mem_heapsize(void);   /* the heap's current size in bytes */

/* Used by the driver only. */
void mem_init(void);         /* get the region; call once */
void mem_deinit(void);       /* give it back */
void mem_reset_brk(void);    /* empty the heap, before each run of a trace */

#endif
