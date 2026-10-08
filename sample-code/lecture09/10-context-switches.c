// Context switches, counted by the kernel.
//
//   voluntary    the process gave up the CPU itself, because it asked the
//                kernel to wait for something (here: sleeping)
//   involuntary  the kernel took the CPU away, because the timer interrupt
//                fired and another process was waiting to run
//
// Part 1 sleeps 100 times. Part 2 computes without stopping for 2 seconds.
// Run it while the machine is busy (start another copy) and watch the
// involuntary count grow.

#include <stdio.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

static void counts(long *vol, long *invol) {
    struct rusage u;
    getrusage(RUSAGE_SELF, &u);
    *vol = u.ru_nvcsw;
    *invol = u.ru_nivcsw;
}

int main(void) {
    long v0, i0, v1, i1;

    counts(&v0, &i0);
    for (int k = 0; k < 100; k++)
        usleep(1000);                   // 1 ms: ask the kernel to wake us later
    counts(&v1, &i1);
    printf("sleeping 100 times:  %4ld voluntary  %4ld involuntary\n", v1 - v0, i1 - i0);

    counts(&v0, &i0);
    volatile unsigned long spin = 0;
    time_t end = time(NULL) + 2;
    while (time(NULL) < end)            // never asks the kernel for anything slow
        spin++;
    counts(&v1, &i1);
    printf("spinning 2 seconds:  %4ld voluntary  %4ld involuntary\n", v1 - v0, i1 - i0);
    return 0;
}
