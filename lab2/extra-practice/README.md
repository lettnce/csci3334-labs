# Lab 2 Extra Practice: code

This folder goes with **lab2-asm.pdf** (Lab 2 Extra Practice Problems). Each
problem in Part One gives you an assembly listing. You write C that behaves the
same way, and this code tells you whether it does.

| File | What it is |
|---|---|
| `mine.c` | **Your answers.** One empty function per problem, named `my_...`. |
| `listings.s` | Every Part One listing from the PDF, as real assembly. |
| `tests.c` | Runs your function and the listing on the same inputs and compares. |
| `Makefile` | `make test` builds and runs everything. |

You need **Linux x86-64**, the same setup you use for Lab 2. It will not build
on a Mac.

## How to use it

1. Pick a problem in the PDF, for example Easy 1, *Three-Way Add*.
2. Write your C in `mine.c`, in `my_add3`.
3. Test just that one:

   ```
   make test F=add3
   ```

4. `PASS` means your C matches the assembly on every test input. `FAIL` shows
   up to three inputs where they differ, with both answers.

`make test` with no `F=` tests all eighteen. Functions you have not written yet
return 0, so they show as `FAIL` until you do.

The function names for `F=` are: add3, affine, times12, first_plus_third,
swap, byte_sum, max2, is_below, abs_diff, power, find, dot, div8, grid_get,
list_sum, str_eq, apply, fib.

If your function calls one of the others (Hard 6, `fib`, calls itself), call
the `my_` version. Plain `fib` is the packet's assembly, so calling it would
pass without testing your code.

Part Three of the PDF has full solutions. Try each problem first, then compare.
