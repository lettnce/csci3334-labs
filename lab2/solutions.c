/*
 * Lab 2, Part 1 — Reverse Engineering (60 pts + 10 bonus)
 *
 * You are given a warmup, mystery0.o, and five compiled functions,
 * mystery1.o .. mystery5.o. Work out
 * what each one does and write a C function here that behaves identically.
 *
 * How to look at one:
 *
 *     objdump -d mystery0.o                 disassemble
 *     make test P=0                          check just your mystery0
 *     make test                              check all five
 *
 * The signatures are given — that part is free. What each function *computes*
 * is what you have to work out.
 *
 * mystery0 is a warmup worth 0 points. mystery1..mystery4 are 15 points each,
 * and mystery5 is a +10 bonus. Each is checked against the real .o over a
 * large range of inputs. They get harder in order; do them in order.
 *
 * Reading assembly is a skill you build by doing it slowly the first few
 * times. Print the disassembly, mark the loop, name the registers on paper.
 */

/*
 * mystery0 — WARMUP, 0 points. Takes two ints, returns an int.
 *
 * Do this one first, even though it scores nothing: it is two instructions,
 * no loop and no branch, so all you are practising is the workflow.
 *
 *   1. a arrives in %edi, b in %esi; the answer leaves in %eax.
 *   2. lea computes an address without touching memory, so read
 *      lea D(B,I,S), dst as plain arithmetic: dst = B + I*S + D.
 *   3. Write out what %eax holds after the first instruction, then after the
 *      second, in terms of a and b.
 *   4. make test P=0 should print PASS.
 */
int my_mystery0(int a, int b)
{
    /* YOUR CODE HERE */
    (void)a;
    (void)b;
    return 0;
}

/*
 * mystery1 — takes an int, returns an int.
 *
 * Hint for your first one: find the loop. Which register accumulates, and
 * which one counts? What is the exit condition?
 */
int my_mystery1(int n)
{
    /* YOUR CODE HERE */
    (void)n;
    return 0;
}

/*
 * mystery2 — takes an unsigned, returns an int.
 *
 * Hint: watch what happens to the argument register inside the loop. A shift
 * plus a mask is usually a loop over bits.
 */
int my_mystery2(unsigned x)
{
    /* YOUR CODE HERE */
    (void)x;
    return 0;
}

/*
 * mystery3 — takes a pointer to int and a count, returns an int.
 *
 * Note the scaling on the pointer arithmetic: `(%rdi,%rax,4)` is indexing an
 * array of 4-byte elements. Also check what happens for small counts: there
 * are two early returns before the loop, and one of them does not return 0.
 */
int my_mystery3(const int *a, int n)
{
    /* YOUR CODE HERE */
    (void)a;
    (void)n;
    return 0;
}

/*
 * mystery4 — takes two ints, returns an int.
 *
 * You will see `cltd` followed by `idivl`. That pair is signed division: it
 * leaves the quotient in %eax and the remainder in %edx. Which one does this
 * function keep?
 *
 * Check the negative cases carefully — something happens to the arguments
 * before the loop starts.
 */
int my_mystery4(int a, int b)
{
    /* YOUR CODE HERE */
    (void)a;
    (void)b;
    return 0;
}

/*
 * mystery5 — takes a pointer to char, returns an int.   *** BONUS ***
 *
 * This one is the bonus; the other four are the required part. Do them first.
 *
 * Convert the constants to ASCII before you guess what they mean. You will
 * find ONE `cmpb` where you might expect two — `subl` then a single unsigned
 * `cmpb`, with `adcl` adding the carry flag, does a two-sided range test with
 * no branch at all. Work out why subtracting first makes one compare enough.
 *
 * There is also a guard before the loop for one particular argument value.
 */
int my_mystery5(const char *s)
{
    /* YOUR CODE HERE */
    (void)s;
    return 0;
}
