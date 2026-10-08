# Sample code

Small, standalone C programs from the lectures. Each lecture has its own
directory so you can compile, run and edit the examples without picking them
out of the slides.

```text
sample code/
  lecture02/   bits, bytes, characters and integers
  lecture03/   addresses, pointers, arrays and strings
  lecture04/   structs, alignment, unions and bit fields
  lecture05/   integer arithmetic, overflow and the bugs it causes
  lecture05b/  floating point
  lecture06/   x86-64 registers, data movement and addressing
  lecture07/   jumps, the stack, calls and recursion (+ a GPU warp-divergence demo)
  lecture08/   buffer overflows, defenses and undefined behaviour
  lecture09/   the kernel: exceptions, system calls and processes (+ a GPU demo)
```

## Lecture 2

The files follow the order of the slides:

| File | Slides |
|------|--------|
| `01-notations.c` | Writing the Same Number Three Ways · Telling `printf` What You Handed It |
| `02-characters.c` | One Byte, Two Readings · Characters Are Just Numbers |
| `03-bitwise.c` | Bitwise Operations, One Column at a Time · De Morgan's Laws · Shifting Bits |
| `04-masks.c` | Test, Set, Clear, Toggle |
| `05-integers.c` | What Two's Complement Means · Sign Extension · Truncation · Size of Integer Types |
| `06-byte-order.c` | Byte Ordering (Endianness) |
| `07-single-number.c` | Bit Tricks in the Wild |

## Lecture 3

Addresses printed by these are whatever that run happened to get — they change
every time you run them, which is itself worth seeing.

| File | Slides |
|------|--------|
| `01-pointers.c` | Pointers Demystified · Three Things, Kept Straight · Two Pointers, One Object |
| `02-arrays.c` | Pointer Arithmetic · An Array Is Not a Pointer · Arrays Decay to Pointers |
| `03-strings.c` | Strings Are Char Arrays |
| `04-2d-arrays.c` | Two-Dimensional Arrays · Why the Order Matters |
| `05-stack.c` | The Stack |
| `06-double-pointers.c` | Why `char **argv` · A Matrix, Two Ways |

## Lecture 4

| File | Slides |
|------|--------|
| `00-print-binary.c` | Warm-Up: Printing a Number in Binary · Run It: Old Friends in Binary |
| `01-structs.c` | Three Arrays, or One? · Filling One In |
| `02-pointers.c` | Structs Through a Pointer · Passing One to a Function |
| `03-layout.c` | The `offsetof` Macro · An Array Multiplies It |
| `04-nested.c` | Structs Inside Structs |
| `05-union.c` | A Union Holds One Member at a Time · Why You Would Want That · The Other Use: The Same Bytes, Read Twice |
| `06-file-header.c` | When the Layout Is Not Yours to Choose |
| `07-aos-soa.c` | AI Systems Connection |
| `08-bitfields.c` | — (Practice 4; bit fields are mentioned, not lectured) |

`00-print-binary.c` is the warm-up. Keep `print_binary` around — it comes back
later in the course.

`07-aos-soa.c` is a timing program, so the Makefile builds it with `-O2`. Your
numbers will not match the slide's; the byte counts will.

## Lecture 5

| File | Slides |
|------|--------|
| `00-rewind.c` | Rewind: The Box From Lecture 2 · The Odometer Rolls Over |
| `01-wrap.c` | Overflow Is a Clock, Not a Crash |
| `01b-promotion.c` | The Size of the Box Decides |
| `02-signed.c` | Adding Two Signed Bytes · Ask Before You Add |
| `03-intmin.c` | `INT_MIN` Has No Twin |
| `04-average.c` | The Midpoint That Overflowed · Run It: A Negative Array Index |
| `05-compare.c` | Signed and Unsigned: Do Not Guess · Run It: The Check That Let It Through |
| `06-alloc.c` | The Allocation That Wraps |
| `07-shift.c` | Division · Shifting a Negative Is Not Dividing It |
| `08-quantize.c` | AI Systems Connection · Run It: The Accumulator That Wrapped |
| `09-ubsan.c` | Find It Before It Finds You |

`00-rewind.c` reuses `print_binary` from lecture 4, narrowed to one byte.

`01b-promotion.c` is built for running live: four numbered sections, and a
header comment listing types to swap in and what each one changes.

`09-ubsan.c` is deliberately broken. `make ubsan` builds the sanitized version:

```bash
make ubsan && ./09-ubsan-san 2147483647 1
```

`05-compare.c` switches `-Wsign-compare` off with a pragma so the bug can run
at all. That warning is the real defence — never write the pragma yourself.

## Lecture 5b

| File | Slides |
|------|--------|
| `00-float-bits.c` | See the Bits Yourself |
| `01-bits.c` | Pulling the Three Fields Apart · Run It: Eight Values, Thirty-Two Bits |
| `02-spacing.c` | Run It: The Gap Grows With the Number · Where a Float Stops Counting |
| `03-not-point-one.c` | Run It: What 0.1 Actually Holds · Never Compare Floats with `==` |
| `04-rounding.c` | Three Ways to Lose the Fraction |
| `05-special.c` | Run It: The Ends of the Range |
| `06-order.c` | Floating Point Is Not Real Arithmetic · Run It: Same Numbers, Different Order |
| `07-bf16.c` | BF16 Is FP32 With the Tail Cut Off |

Lecture 5b's Makefile links `-lm` for `nextafterf`, `fabs` and `isnan`.

`bit-basics.c` is a scratch file that touches a bit of everything — handy for
experimenting, not tied to any one slide.

## Building

Everything at once, from the lecture directory:

```bash
make        # build them all
make run    # build, then run each in order
make clean  # remove the binaries
```

Or one at a time:

```bash
gcc -std=c17 -Wall -Wextra -Werror 03-bitwise.c -o 03-bitwise
./03-bitwise
```

Every example compiles clean with those flags. If yours does not, the warning
is telling you something — read it before you silence it.

## Lecture 6

These are the first programs meant to be **read as assembly**, not only run.
`make asm` emits AT&T-syntax x86-64; `make dis` adds the byte encodings.
Both work on an Apple Silicon Mac — clang cross-compiles — though the x86-64
binaries themselves will not run there.

| File | Slides |
|------|--------|
| `01-first-look.c` | Run It: Get the Assembly Yourself · Run It: One Line of C, One Addressing Mode · Your First Whole Function · A Loop, Line by Line · Run It: The Bytes Are Right There |
| `02-lea.c` | Run It: `lea` as the Cheap Multiplier |
| `03-shapes.c` | The Four Shapes, in a Register |
| `04-sizes.c` | Run It: Writing Through the Narrow Names |

## Lecture 7

| File | Slides |
|------|--------|
| `01-branches.c` | Run It: Same Bits, Two Answers |
| `02-loops.c` | Run It: `while` and `for` Are the Same Code |
| `03-cmov.c` | Run It: The Compiler Decides, Not Your Source · When `cmov` Is Not Allowed |
| `04-calls.c` | Run It: Print the Return Address |
| `05-recursion.c` | Run It: Watch the Stack March Down |

`./05-recursion 200000 > /dev/null` overflows the stack and dies with SIGSEGV —
the fastest way to see a stack overflow happen.

## Lecture 8

Two extra targets here, because the point of both is that the build settings
change the answer:

```bash
make asan    # AddressSanitizer build of 01-overflow
make ub      # builds 03-ub at -O0 AND -O1 and runs both
```

| File | Slides |
|------|--------|
| `01-overflow.c` | Run It: AddressSanitizer Finds It |
| `02-strncpy.c` | Run It: `strncpy` Does Not Terminate |
| `03-ub.c` | Run It: The Check That Disappeared |
| `04-aliasing.c` | Run It: Two Defined Ways to Read the Bits |
| `05-aslr.c` | Run It: Nothing Is Where It Was |

`01-overflow.c` is deliberately buggy — it is the bug the lecture is about.
Run it under `make asan` to see the overflow named.

## Lecture 9

This lecture is about Linux itself, so several demos need Linux (WSL2, a
Linux VM, or the lab machines). `strace` and `/proc` exist only on Linux.

```bash
make          # everything that runs on any machine
make run
make linux    # 01-privileged and 04-hello: Linux x86-64 only
make gpu      # 11-launches: needs an NVIDIA GPU and nvcc
```

| File | Slides |
|------|--------|
| `01-privileged.c` | Run It: A Forbidden Instruction (Linux x86-64) |
| `02-page-faults.c` | Run It: Counting Page Faults |
| `03-divide.c` | Run It: Dividing by Zero on Two CPUs |
| `04-hello.s` | Hello, World With No C Library · Run It: Two System Calls, Nothing Else (Linux x86-64) |
| `05-errno.c` | How a System Call Reports an Error |
| `06-printf.c` | Practice II, question 8 (run it under `strace`) |
| `07-buffering.c` | Run It: 1000 `printf` Calls, How Many `write`s? |
| `08-syscall-cost.c` | Run It: What a System Call Costs |
| `09-pid.c` | Run It: Every Process Has a Number |
| `10-context-switches.c` | Run It: Counting Context Switches (use Linux; macOS counts differently) |
| `11-launches.cu` | AI Systems Connection |

`01-privileged` and `03-divide` are meant to be killed by the kernel. That is
what they demonstrate.

