// printf inside a GPU kernel. The GPU cannot make system calls, so who calls
// write(), and when?
//
//   ./12-gpu-printf                   4 GPU threads print, then the CPU waits
//   ./12-gpu-printf 4 nowait          the CPU never waits for the GPU
//   ./12-gpu-printf 1048576 | wc -l   a million GPU threads print one line each
//   strace -f -e trace=write ./12-gpu-printf
//
// Needs an NVIDIA GPU:   make gpu

#include <cstdio>
#include <cstdlib>
#include <cstring>

__global__ void hello(void) {
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    printf("GPU thread %d: hello\n", id);
}

int main(int argc, char **argv) {
    int n = argc > 1 ? atoi(argv[1]) : 4;
    int wait = !(argc > 2 && strcmp(argv[2], "nowait") == 0);
    int per_block = n < 256 ? n : 256;

    hello<<<(n + per_block - 1) / per_block, per_block>>>();
    printf("CPU: kernel launched\n");
    fflush(stdout);

    if (wait)
        cudaDeviceSynchronize();      // the CPU waits for the GPU here
    printf("CPU: done\n");
    return 0;
}
