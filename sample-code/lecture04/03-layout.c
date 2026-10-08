/* Lecture 4: ask the compiler where every field landed.
 *
 * offsetof is in <stddef.h>. It reports the real byte offset, so you never
 * have to trust your arithmetic — or your memory of the rules.
 */
#include <stddef.h>
#include <stdio.h>

struct Bad  { char c; int i; short s; };
struct Good { int i; short s; char c; };

int main(void)
{
    printf("struct Bad   sizeof %2zu  align %zu\n",
           sizeof(struct Bad), _Alignof(struct Bad));
    printf("  c at %zu\n", offsetof(struct Bad, c));
    printf("  i at %zu\n", offsetof(struct Bad, i));
    printf("  s at %zu\n", offsetof(struct Bad, s));

    printf("\nstruct Good  sizeof %2zu  align %zu\n",
           sizeof(struct Good), _Alignof(struct Good));
    printf("  i at %zu\n", offsetof(struct Good, i));
    printf("  s at %zu\n", offsetof(struct Good, s));
    printf("  c at %zu\n", offsetof(struct Good, c));

    /* The padding is not paid once. It is paid per element. */
    enum { N = 1000000 };
    printf("\n%d records of Bad   %8zu bytes\n", N, N * sizeof(struct Bad));
    printf("%d records of Good  %8zu bytes\n", N, N * sizeof(struct Good));
    printf("difference          %8zu bytes\n",
           N * (sizeof(struct Bad) - sizeof(struct Good)));
    return 0;
}
