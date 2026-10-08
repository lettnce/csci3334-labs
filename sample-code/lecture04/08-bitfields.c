/* Lecture 4: bit fields name pieces of a storage unit smaller than a byte.
 *
 * They are convenient inside your own program. They are a poor choice for a
 * wire format, because which end the compiler fills first is
 * implementation-defined — this program shows you which end yours picked.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* The first four bytes of an IPv4 header. */
struct IPv4Head {
    unsigned ihl     : 4;
    unsigned version : 4;
    unsigned tos     : 8;
    unsigned length  : 16;
};

struct Plain { unsigned ihl, version, tos, length; };

int main(void)
{
    printf("struct IPv4Head  %zu bytes   (32 bits of fields)\n",
           sizeof(struct IPv4Head));
    printf("struct Plain     %zu bytes   (the same four fields, unpacked)\n\n",
           sizeof(struct Plain));

    struct IPv4Head h = { .version = 4, .ihl = 5, .tos = 0, .length = 60 };
    uint8_t raw[sizeof h];
    memcpy(raw, &h, sizeof h);

    printf("version=%u ihl=%u tos=%u length=%u\n", h.version, h.ihl, h.tos, h.length);
    printf("bytes in memory:");
    for (size_t i = 0; i < sizeof raw; i++)
        printf(" %02X", raw[i]);
    printf("\n");
    printf("\non the wire an IPv4 header of length 60 reads  45 00 00 3C\n");
    printf("this machine wrote                            ");
    for (size_t i = 0; i < sizeof raw; i++)
        printf("%02X ", raw[i]);
    printf("\nbyte 0 matched by luck; the 16-bit length did not.\n\n");

    /* By hand, with shifts and masks: no layout to be surprised by. */
    uint8_t byte0 = (uint8_t)((4u << 4) | 5u);
    printf("built by hand:  %02X\n", byte0);
    return 0;
}
