#include <iostream>
#include <cuda_runtime.h>
#include <ctime>

using namespace std;

void cpuSum(int* A, int* B, int* C, int N) {
    for (int i = 0; i < N; ++i) {
        C[i] = A[i] + B[i];
    }
}

//gpu kermel
__global__ void kernel(int* A, int* B, int* C, int N)
{
    int i= blockIdx.x * blockDim.x + threadIdx.x;
    if(i<N)
    {
        C[i] = A[i] + B[i];
    }

}

//gpu wrapper

void gpusum(int* d_A, int* d_B, int* d_C, int N)
{
    int threadsperblock = 1024;
    int blockpergrid = (N + threadsperblock - 1)/threadsperblock;
    kernel<<<blockpergrid, threadsperblock>>>(d_A, d_B, d_C, N);
}

bool isEqual(int* A, int* B, int N) {
    for (int i = 0; i < N; i++) {
        if (A[i] != B[i]) return false;
    }
    return true;
}


int main()
{
    int N = 2e7;
    int size = N * sizeof(int);

    int *A = (int*)malloc(size);
    int *B = (int*)malloc(size);
    int *C = (int*)malloc(size);
    int *D = (int*)malloc(size);
    
    srand(time(0));

    for (int i = 0; i < N; i++) {
        A[i] = rand() % 1000;
        B[i] = rand() % 1000;
    }
    

    clock_t start = clock();
    cpuSum(A, B, C, N);
    clock_t end = clock();
    float cpuTime = (float)(end - start) / CLOCKS_PER_SEC;

    int *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, size);
    cudaMalloc(&d_B, size);
    cudaMalloc(&d_C, size);

    cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);
    

    cudaEvent_t startevent, stopevent;
    cudaEventCreate(&startevent);
    cudaEventCreate(&stopevent);

    cudaEventRecord(startevent);
    gpusum(d_A, d_B, d_C, N);
    cudaEventRecord(stopevent);

    cudaEventSynchronize(stopevent);

    float gpuTimeMs = 0;
    cudaEventElapsedTime(&gpuTimeMs, startevent, stopevent);

    cudaMemcpy(D, d_C, size, cudaMemcpyDeviceToHost);

    bool success = isEqual(C, D, N);

    cout << "Vector Addition\n";
    cout << "----------------------\n";
    cout << "CPU Time: " << cpuTime << " sec\n";
    cout << "GPU Kernel Time: " << gpuTimeMs / 1000.0 << " sec\n";
    cout << "Speedup: " << cpuTime / (gpuTimeMs / 1000.0) << "\n";
    cout << "Verification: " << (success ? "true" : "false") << "\n";

    // Free memory
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    free(A);
    free(B);
    free(C);
    free(D);

    return 0;
    
}

