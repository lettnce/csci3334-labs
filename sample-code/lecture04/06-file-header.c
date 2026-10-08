/* Lecture 4: when the layout is not yours to choose.
 *
 * A BMP file starts with a 14-byte header. Declare it as the obvious struct
 * and the compiler pads it to 16 — so reading the file straight into it puts
 * every field after the first two bytes in the wrong place.
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct BmpNaive {
    char     magic[2];      /* "BM"                    */
    uint32_t size;          /* file size in bytes      */
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;        /* where the pixels start  */
};

/* Same fields, told not to pad. Now sizeof is right — but every uint32_t in
   it is unaligned, which costs speed and, on some machines, crashes. */
struct __attribute__((packed)) BmpPacked {
    char     magic[2];
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
};

/* The portable answer: take the bytes and assemble the numbers yourself.
   No layout assumptions, no byte-order assumptions. */
static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8
         | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

int main(void)
{
    /* The first 14 bytes of a real 1x1 BMP. */
    const uint8_t header[14] = {
        'B', 'M', 0x3A, 0x00, 0x00, 0x00, 0, 0, 0, 0, 0x36, 0x00, 0x00, 0x00
    };

    printf("the file's header is  %zu bytes\n", sizeof header);
    printf("struct BmpNaive is    %zu bytes   size at offset %zu\n",
           sizeof(struct BmpNaive), offsetof(struct BmpNaive, size));
    printf("struct BmpPacked is   %zu bytes   size at offset %zu\n\n",
           sizeof(struct BmpPacked), offsetof(struct BmpPacked, size));

    struct BmpNaive bad;
    memcpy(&bad, header, sizeof bad > 14 ? 14 : sizeof bad);
    printf("read into BmpNaive:  size = %u   (wrong)\n", bad.size);

    printf("read byte by byte:   size = %u   offset = %u\n",
           le32(header + 2), le32(header + 10));
    return 0;
}
