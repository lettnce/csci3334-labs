/* Lecture 4: nesting, and where the padding goes when structs contain structs. */
#include <stddef.h>
#include <stdio.h>

struct Inner { char c; int i; };          /* 8 bytes, alignment 4 */

struct Outer {
    char         tag;                     /* 0                    */
    struct Inner in;                      /* 4, because Inner needs 4 */
    char         end;                     /* 12                   */
};                                        /* rounded up to 16     */

struct Wide { char c; double d; };        /* alignment 8, so 16 bytes */

int main(void)
{
    printf("Inner  sizeof %2zu  align %zu   c at %zu, i at %zu\n",
           sizeof(struct Inner), _Alignof(struct Inner),
           offsetof(struct Inner, c), offsetof(struct Inner, i));

    printf("Outer  sizeof %2zu  align %zu   tag at %zu, in at %zu, end at %zu\n",
           sizeof(struct Outer), _Alignof(struct Outer),
           offsetof(struct Outer, tag), offsetof(struct Outer, in),
           offsetof(struct Outer, end));

    printf("Wide   sizeof %2zu  align %zu   c at %zu, d at %zu\n",
           sizeof(struct Wide), _Alignof(struct Wide),
           offsetof(struct Wide, c), offsetof(struct Wide, d));

    /* A nested struct is not flattened. Outer's alignment is the largest
       alignment anywhere inside it, however deep. */
    printf("\nOuter.in.i sits at %zu\n",
           offsetof(struct Outer, in) + offsetof(struct Inner, i));
    return 0;
}
