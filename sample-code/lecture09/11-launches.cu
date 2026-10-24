// What does it cost to launch a GPU kernel, and when does that cost matter?
//
// 1. Launch an empty kernel 100,000 times and divide: the cost of one launch.
// 2. Do one million additions two ways:
//      1,000 small kernels that each add 1 to 1,000 numbers, or
//      1 big kernel that adds 1 to a million numbers.
//    Same arithmetic. The only difference is the number of launches.
//
// Needs an NVIDIA GPU:   make gpu   (or: nvcc -O2 -arch=native 11-launches.cu -o 11-launches)
//   ./11-launches

#include <chrono>
#include <cstdio>

__global__ void empty(void) { }

__global__ void add_one(float *x, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n)
        x[i] += 1.0f;
}

static double now_us(void) {
    using namespace std::chrono;
    return duration<double, std::micro>(steady_clock::now().time_since_epoch()).count();
}

int main(void) {
    const int SMALL = 1000, LAUNCHES = 1000, BIG = SMALL * LAUNCHES;
    float *x;
    cudaMalloc(&x, BIG * sizeof(float));
    cudaMemset(x, 0, BIG * sizeof(float));

    empty<<<1, 1>>>();                    // the first launch sets up the driver
    cudaDeviceSynchronize();

    // 1. the cost of one launch
    const int N = 100000;
    double t0 = now_us();
    for (int i = 0; i < N; i++)
        empty<<<1, 1>>>();
    cudaDeviceSynchronize();
    double t1 = now_us();
    printf("one launch of an empty kernel:      %6.2f us\n", (t1 - t0) / N);

    // 2. the same million additions, as 1,000 small kernels or 1 big one
    add_one<<<(BIG + 255) / 256, 256>>>(x, BIG);    // warm up both shapes
    add_one<<<(SMALL + 255) / 256, 256>>>(x, SMALL);
    cudaDeviceSynchronize();

    t0 = now_us();
    for (int k = 0; k < LAUNCHES; k++)
        add_one<<<(SMALL + 255) / 256, 256>>>(x, SMALL);
    cudaDeviceSynchronize();
    t1 = now_us();
    printf("1,000 small kernels (1,000 adds each): %8.1f us\n", t1 - t0);

    t0 = now_us();
    add_one<<<(BIG + 255) / 256, 256>>>(x, BIG);
    cudaDeviceSynchronize();
    t1 = now_us();
    printf("1 big kernel (1,000,000 adds):         %8.1f us\n", t1 - t0);

    cudaFree(x);
    return 0;
}
