/*
 * mm.h - the functions you write in mm.c. Do not change this file.
 */
#ifndef MM_H
#define MM_H

#include <stddef.h>

int   mm_init(void);                         /* start a new, empty heap; 0 on success */
void *mm_malloc(size_t size);                /* Level 1 */
void  mm_free(void *ptr);                    /* Level 1, coalescing at Level 2 */
void *mm_realloc(void *ptr, size_t size);    /* Level 3 */
void *mm_calloc(size_t nmemb, size_t size);  /* Level 3 */
int   mm_check(void);                        /* Level 5: 0 if the heap is consistent */
void  mm_print_heap(void);                   /* provided: prints every block */

#endif
