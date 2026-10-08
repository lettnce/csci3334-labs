// Warp divergence: the same if, the same work, a different split.
//
// A GPU runs threads in groups of 32 called warps. All 32 threads in a warp
// execute the same instruction at the same time. If an `if` sends some of
// them one way and the rest the other way, the warp runs the first path with
// the other threads switched off, then runs the second path. It pays for both.
//
// All three kernels below do the same amount of arithmetic per thread. Only
// WHICH threads take which path changes:
//
//   no_branch      every thread runs path A            (the baseline)
//   warp_aligned   warp 0 runs A, warp 1 runs B, ...   (no warp is split)
//   divergent      even threads run A, odd run B       (EVERY warp is split)
//
// Needs an NVIDIA GPU:   make gpu   (or: nvcc -O2 -arch=native 06-divergence.cu -o 06-divergence)

#include <cstdio>
#include <cstdlib>

#define N     (1 << 22)     // 4M threads
#define ITERS 4096          // work per path

// Two different loops, so the compiler cannot merge them into one.
__device__ float path_a(float x) {
    #pragma unroll 1
    for (int i = 0; i < ITERS; i++) x = x * 1.000001f + 0.5f;
    return x;
}

__device__ float path_b(float x) {
    #pragma unroll 1
    for (int i = 0; i < ITERS; i++) x = x * 0.999999f - 0.25f * x;
    return x;
}

__global__ void no_branch(float *out) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    out[i] = path_a((float)i);
}

__global__ void warp_aligned(float *out) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if ((i / 32) % 2 == 0) out[i] = path_a((float)i);   // whole warp agrees
    else                   out[i] = path_b((float)i);
}

__global__ void divergent(float *out) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i % 2 == 0) out[i] = path_a((float)i);          // every warp splits
    else            out[i] = path_b((float)i);
}

static float time_ms(void (*kernel)(float *), float *out) {
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    kernel<<<N / 256, 256>>>(out);                      // warm-up run
    cudaEventRecord(start);
    kernel<<<N / 256, 256>>>(out);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float ms;
    cudaEventElapsedTime(&ms, start, stop);

    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "kernel failed: %s\n", cudaGetErrorString(err));
        exit(1);
    }
    return ms;
}

int main(void) {
    float *out;
    if (cudaMalloc(&out, N * sizeof(float)) != cudaSuccess) {
        fprintf(stderr, "no CUDA device\n");
        return 1;
    }

    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    printf("%s, warp size %d, %d threads\n\n", prop.name, prop.warpSize, N);

    float base  = time_ms(no_branch, out);
    float align = time_ms(warp_aligned, out);
    float split = time_ms(divergent, out);

    printf("  no_branch     %7.2f ms   every thread runs path A\n", base);
    printf("  warp_aligned  %7.2f ms   half the warps run A, half run B\n", align);
    printf("  divergent     %7.2f ms   every warp runs A, then B\n", split);
    printf("\ndivergent is %.1fx slower than warp_aligned, for the same work.\n",
           split / align);

    cudaFree(out);
    return 0;
}
