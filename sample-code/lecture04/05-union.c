/* Lecture 4: a union gives one region of memory more than one name.
 *
 * A struct puts its members side by side, so it holds all of them at once.
 * A union puts them on top of each other: every member starts at offset 0,
 * the union is as big as its largest member, and only one member is valid at
 * a time. Which one? Nothing in the union says. That is what the tag is for.
 */
#include <stdint.h>
#include <stdio.h>

/* ---- use one: a value that is one of several shapes ------------------- */

struct Everything {                 /* all three, always */
    enum { INT, REAL, TEXT } kind;
    long   i;
    double d;
    char  *s;
};

struct Value {                      /* one at a time, and a tag to say which */
    enum { V_INT, V_REAL, V_TEXT } kind;
    union {
        long   i;
        double d;
        char  *s;
    } as;
};

static void print_value(struct Value v)
{
    switch (v.kind) {               /* read only the member the tag names */
    case V_INT:  printf("int    %ld\n", v.as.i);   break;
    case V_REAL: printf("real   %g\n",  v.as.d);   break;
    case V_TEXT: printf("text   %s\n",  v.as.s);   break;
    }
}

/* ---- use two: the same bytes, read a second way ----------------------- */

union Bits {
    float    f;
    uint32_t u;
    uint8_t  b[4];
};

int main(void)
{
    printf("struct Everything  %zu bytes   all three members, always\n",
           sizeof(struct Everything));
    printf("struct Value       %zu bytes   a tag plus one member\n\n",
           sizeof(struct Value));

    struct Value vals[] = {
        { .kind = V_INT,  .as.i = 42 },
        { .kind = V_REAL, .as.d = 3.5 },
        { .kind = V_TEXT, .as.s = "hola" },
    };
    for (size_t k = 0; k < sizeof vals / sizeof vals[0]; k++)
        print_value(vals[k]);

    /* Write one member, read another, and you get the same bytes read a
       second way. In C this is well defined; in C++ it is not. */
    union Bits v;
    v.f = 3.0f;
    printf("\nv.f = %g   v.u = 0x%08X\n", (double)v.f, v.u);

    printf("bytes, lowest address first:");
    for (int i = 0; i < 4; i++)
        printf(" %02X", v.b[i]);
    printf("\n");

    /* Lecture 2's endianness test, written with a union instead of a cast. */
    union { uint32_t u; uint8_t b[4]; } probe = { .u = 0x01020304 };
    printf("\n0x01020304 stored as %02X %02X %02X %02X  -> %s-endian\n",
           probe.b[0], probe.b[1], probe.b[2], probe.b[3],
           probe.b[0] == 0x04 ? "little" : "big");

    /* The tag is not optional. Nothing stops you reading the wrong member,
       and nothing warns you either -- these are integer bits read as a
       double, and the number is meaningless. */
    union Bits oops;
    oops.u = 42;
    printf("\nwrote u = 42, read f -> %g   (nobody stopped us)\n", (double)oops.f);
    return 0;
}
