/* Lecture 4: a struct is several values given one name. */
#include <stdio.h>
#include <string.h>

struct Student {
    char  name[16];
    int   id;
    float gpa;
};

int main(void)
{
    /* Three ways to fill one in. The third names the fields, so it does not
       care what order they were declared in — prefer it. */
    struct Student a;
    strcpy(a.name, "Ana");
    a.id  = 1001;
    a.gpa = 3.7f;

    struct Student b = { "Beto", 1002, 3.1f };
    struct Student c = { .name = "Carmen", .gpa = 4.0f, .id = 1003 };

    struct Student roster[3] = { a, b, c };

    for (int i = 0; i < 3; i++)
        printf("%-8s %d  %.2f\n",
               roster[i].name, roster[i].id, (double)roster[i].gpa);

    /* Assignment copies the whole struct, arrays inside it included. This is
       the one place C copies an array for you. */
    struct Student copy = roster[0];
    strcpy(copy.name, "Anita");
    printf("\nafter editing the copy: %s and %s\n", roster[0].name, copy.name);
    return 0;
}
