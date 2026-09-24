/*
 * Lab 1: Data Representation — Bit Manipulation
 *
 * Rules:
 *   - Only allowed: ! ~ & ^ | + << >>
 *   - No loops, conditionals, macros (unless stated)
 *   - No constants larger than 0xFF
 *   - Integer constants 0..255 (0x00..0xFF)
 *   - No casting (unless stated for float problems)
 *   - Max ops per function shown in comment
 *
 * Replace each "return 2;" with your solution.
 */

/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {

    // 4 - 0100
    // 5 - 0101
    // 1 - 0001
    int step_1 = (~x&y);
    int step_2 = (x&~y);
    return ~(~step_1&~step_2);
}

/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    return 1 << 31;
}

/*
 * isTmax - returns 1 if x is the maximum two's complement integer,
 *   and 0 otherwise          // 0111 1111 ... 1111 1111
 *   Legal ops: ! ~ & ^ | +   // 1000 0000 ... 0000 0000
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x){
    int neg1 = !(~x);
    return !(~(x + 1) ^ x | neg1); 
}

/*
 * allOddBits - returns 1 if all odd-numbered bits in word are 1
 *   Bits are numbered from 0 (LSB) to 31 (MSB)
 *   Examples: allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) { // 2, 8, 32, 128, 512, 2048, etc...
    int mask = 0xAA | (0xAA << 8);
    mask = mask | (mask << 16);
    return !((x & mask)^mask);
}

/*
 * negate - return -x
 *   Example: negate(1) = -1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    return ~x + 1;
}

/*
 * isAsciiDigit - returns 1 if 0x30 <= x <= 0x39 (ASCII '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1, isAsciiDigit(0x3a) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    int high = !((x >> 4) ^ 0x3);
    int low = !(((x & 0xF) + 0x6) >> 4);
    return high & low;
}

/*
 * conditional - same as x ? y : z
 *   Example: conditional(2, 4, 5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    int mask = (!x) + ~0;
    return (mask & y) | (~mask & z);
}

/*
 * isLessOrEqual - if x <= y then return 1, else return 0
 *   Example: isLessOrEqual(4, 5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int x_sign_bit = (x >> 31) & 1;
    int y_sign_bit = (y >> 31) & 1;
    // if sign bits differ, x smaller when negative
    // if same, inspect y + (~x + 1)
    int calc = y + (~x + 1);
    int alt_sign_bit = (calc >> 31) & 1;
    int diff_or_equal = x_sign_bit ^ y_sign_bit;

    int not_diff = diff_or_equal ^ 1;   // logical NOT (operands are 0/1)
    int not_alt  = alt_sign_bit ^ 1;

    return (diff_or_equal & x_sign_bit) | (not_diff & not_alt);
}

/*
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f. Both the argument and result are
 *   passed as unsigned ints, but they are interpreted as the bit-level
 *   representation of single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    unsigned exp  = uf & 0x7F800000;
    unsigned frac = uf & 0x007FFFFF;
    if (exp == 0x7F800000 && frac != 0) return uf;
    // NaN: return unchanged
    // flip sign bit
    return uf ^ 0x80000000;                          
}

/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but interpreted as
 *   single-precision floating point.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    return 2;
}

/*
 * howManyBits - return the minimum number of bits required to
 *   represent x in two's complement
 *   Examples:
 *     howManyBits(12) = 5      (01100)
 *     howManyBits(298) = 10    (0100101010)
 *     howManyBits(-5) = 4      (1011)
 *     howManyBits(0)  = 1
 *     howManyBits(-1) = 1
 *     howManyBits(0x80000000) = 32
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int howManyBits(int x) {
    int b16, b8, b4, b2, b1, b0;
    int sign = x >> 31;
    x = (sign & ~x) | (~sign & x);   
    // invert if negative, else keep

    b16 = !!(x >> 16) << 4;  
    x = x >> b16;
    b8  = !!(x >> 8)  << 3;  
    x = x >> b8;
    b4  = !!(x >> 4)  << 2;  
    x = x >> b4;
    b2  = !!(x >> 2)  << 1;  
    x = x >> b2;
    b1  = !!(x >> 1);        
    x = x >> b1;
    b0  = x;

    return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}

/* float_f2i - Return bit-level equivalent of (int) f for floating point
 *   argument f. Argument is passed as unsigned int, but interpreted as
 *   the bit-level representation of single-precision floating point.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
    return 2;
}
