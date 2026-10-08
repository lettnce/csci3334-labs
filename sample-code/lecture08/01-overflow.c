// A buffer overflow, and the tool that finds it.
//
// This program is DELIBERATELY BUGGY. It is the bug the lecture is about.
//
//     make asan            build with AddressSanitizer
//     ./01-overflow-asan hello              fine
//     ./01-overflow-asan AAAAAAAAAAAAAAAA   ASAN reports the overflow
//
// Without ASAN a short overflow often "works" -- which is the dangerous part.
// The bug does not announce itself; it corrupts whatever came next.

#include <stdio.h>
#include <string.h>

static void vulnerable(const char *input) {
    char buf[8];

    // The bug: strcpy stops at the input's NUL byte, not at the end of buf.
    // How much it writes is a property of the INPUT, not of the buffer.
    strcpy(buf, input);

    printf("  buf = %s\n", buf);
}

static void safe(const char *input) {
    char buf[8];

    // snprintf always writes at most sizeof(buf) bytes INCLUDING the NUL.
    snprintf(buf, sizeof buf, "%s", input);

    printf("  buf = %s   (truncated to fit)\n", buf);
}

int main(int argc, char **argv) {
    const char *in = (argc > 1) ? argv[1] : "hello";

    printf("input is %zu bytes, buf holds 8\n\n", strlen(in));

    puts("safe(): snprintf bounds the write");
    safe(in);

    puts("\nvulnerable(): strcpy does not");
    vulnerable(in);

    puts("\nIf that did not crash, it still overwrote something.");
    return 0;
}
