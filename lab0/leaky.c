#include <stdio.h>
#include <stdlib.h>

/* This program leaks memory. Run it under Valgrind, find the leak, and fix it. */
int *create_numbers(int n) {
    int *arr = malloc(n * sizeof(int));
    if (!arr) return NULL;
    for (int i = 0; i < n; i++) {
        arr[i] = i * i;
    }
    return arr;  /* the caller now owns this allocation */
}

int main(void) {
    for (int j = 0; j < 10; j++) {
        int *nums = create_numbers(100);
        printf("Iteration %d: first = %d, last = %d\n",
               j, nums[0], nums[99]);
	free(nums);
    }
    return 0;
}
