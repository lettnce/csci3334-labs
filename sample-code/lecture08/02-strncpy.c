// The "safe" function that is not safe.
//
// strncpy is the classic wrong fix for strcpy. It bounds the write, but it
// does NOT guarantee a terminating NUL -- if the source is exactly as long as
// the buffer or longer, you get an unterminated char array, and the next
// thing that reads it runs off the end.

#include <stdio.h>
#include <string.h>

int main(void) {
    const char *src = "0123456789";     // 10 chars + NUL

    // --- the trap ---------------------------------------------------------
    char bad[8];
    memset(bad, '#', sizeof bad);       // fill so we can SEE what lands
    strncpy(bad, src, sizeof bad);      // writes 8 chars, NO terminator

    printf("strncpy into char[8]:\n");
    printf("  bytes:   ");
    for (size_t i = 0; i < sizeof bad; i++) putchar(bad[i]);
    printf("\n  no NUL in those 8 bytes -- printing it as a string is UB.\n");

    // --- the fix ----------------------------------------------------------
    char good[8];
    snprintf(good, sizeof good, "%s", src);

    printf("\nsnprintf into char[8]:\n");
    printf("  string:  %s\n", good);
    printf("  length:  %zu  (7 chars + NUL = 8)\n", strlen(good));

    puts("\nsnprintf always terminates. strncpy only terminates if it had room.");
    puts("That is why the advice is snprintf, not strncpy.");
    return 0;
}
