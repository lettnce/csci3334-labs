/* Lecture 5: the multiplication that turns into a heap overflow.
 *
 * malloc(n * size) looks harmless. If n * size wraps, malloc returns a small
 * block, the loop that fills it writes past the end, and you have handed an
 * attacker the machine. Check the multiplication before you make it.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* WRONG: n * size is computed in size_t, and size_t arithmetic wraps. */
static void *alloc_bad(size_t n, size_t size)
{
    return malloc(n * size);
}

/* RIGHT: division cannot overflow, so ask it first. */
static void *alloc_ok(size_t n, size_t size)
{
    if (size != 0 && n > SIZE_MAX / size)
        return NULL;
    return malloc(n * size);
}

int main(void)
{
    /* volatile so the compiler cannot fold n * size at compile time. GNU gcc
       does spot the constant version and refuses to build it
       (-Walloc-size-larger-than) — a good warning, but it would hide the
       run-time behaviour this program is here to show. */
    volatile size_t vn = (SIZE_MAX / 4) + 1;
    size_t n = vn, size = 8;

    printf("SIZE_MAX      %zu\n", SIZE_MAX);
    printf("n             %zu\n", n);
    printf("n * 8 wraps to %zu bytes\n\n", n * size);

    void *bad = alloc_bad(n, size);
    printf("alloc_bad  -> %s\n",
           bad ? "a pointer to 0 bytes, and no complaint" : "NULL");
    printf("alloc_ok   -> %s\n", alloc_ok(n, size) ? "a block" : "NULL, correctly");

    free(bad);
    /* calloc does this check for you; that is the difference between
       calloc(n, size) and malloc(n * size). */
    printf("\ncalloc(n, 8) -> %s\n", calloc(n, size) ? "a block" : "NULL, correctly");

    /* The same shape with a minus sign. size_t has no negative values, so a
       subtraction that goes below zero lands at the top of the range. */
    size_t capacity = 16, used = 20;
    printf("\ncapacity %zu, used %zu\n", capacity, used);
    printf("capacity - used = %zu\n", capacity - used);
    printf("that > 0        -> %s   <- compare before you subtract\n",
           capacity - used > 0 ? "true" : "false");
    return 0;
}
