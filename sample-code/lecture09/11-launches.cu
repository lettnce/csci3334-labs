// How many system calls does it take to launch a GPU kernel?
//
// Launches an empty kernel N times, then waits for the GPU to finish. Run it
// under strace with two different N and compare the system call counts. If a
// launch needed the OS kernel, the counts would grow with N.
//
//   make gpu
//   strace -f -c -o small.txt ./11-launches 100
//   strace -f -c -o big.txt   ./11-launches 100000
//
// Needs an NVIDIA GPU:   make gpu   (or: nvcc -O2 -arch=native 11-launches.cu -o 11-launches)

#include <cstdio>
#include <cstdlib>

__global__ void empty(void) { }

int main(int argc, char **argv) {
    int n = argc > 1 ? atoi(argv[1]) : 1000;

    empty<<<1, 1>>>();              // the first launch sets up the driver; not counted
    cudaDeviceSynchronize();

    for (int i = 0; i < n; i++)
        empty<<<1, 1>>>();
    cudaError_t err = cudaDeviceSynchronize();

    printf("%d launches: %s\n", n, cudaGetErrorString(err));
    return 0;
}
