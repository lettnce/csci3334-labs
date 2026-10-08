/* Lecture 4: reaching a struct through a pointer, and what a call costs. */
#include <stdio.h>

struct Point {
    double x, y;
    int    tag;
};

/* Takes a copy: 24 bytes pushed for every call, and the caller never sees
   the change. */
static void move_by_value(struct Point p, double dx)
{
    p.x += dx;
}

/* Takes an address: 8 bytes, and it edits the caller's object. */
static void move_by_pointer(struct Point *p, double dx)
{
    p->x += dx;                 /* p->x is exactly (*p).x */
}

/* Read-only, but still cheap to pass. const says so in the signature. */
static void print_point(const struct Point *p)
{
    printf("(%.1f, %.1f) tag %d\n", p->x, p->y, p->tag);
}

int main(void)
{
    struct Point pt = { 1.0, 2.0, 7 };

    printf("sizeof(struct Point) = %zu   sizeof(pointer) = %zu\n\n",
           sizeof pt, sizeof(struct Point *));

    move_by_value(pt, 100.0);
    printf("after move_by_value    ");
    print_point(&pt);

    move_by_pointer(&pt, 100.0);
    printf("after move_by_pointer  ");
    print_point(&pt);

    /* (*p).x and p->x are the same thing. The parentheses are required,
       because . binds tighter than *, which is why -> exists at all. */
    struct Point *p = &pt;
    printf("\n(*p).x = %.1f   p->x = %.1f\n", (*p).x, p->x);
    return 0;
}
