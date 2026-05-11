#include <iostream>
#include <cuda_runtime.h>
#include <ctime>
#include <cmath>

using namespace std;

void cpumatmul(int *A, int *B, int *C, int N)
{
    for(int i=0; i<N; i++)
    {
        for(int j=0; j<N; j++)
        {
            C[i*N+ j] = 0;
            for(int k=0; k<N; k++)
            {
                C[i*N + j]+= A[i*N+k] * B[k* N +j];
            }
        }
    }
}

__global__ void gpumatmul(int *A, int *B, int *C, int N)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col =  blockIdx.x * blockDim.x + threadIdx.x;

    if(row<N && col<N)
    {
        int val = 0;
        for(int k=0; k<N; k++)
        {
            val+= A[row *N + k] * B[k*N + col];
        }
        C[row*N + col] = val;
    }
}

void gpulauncher(int *A, int *B, int *C, int N)
{
    int threadsperblock = 16;
    dim3 threads(threadsperblock, threadsperblock);
    dim3 blocks(ceil(float(N)/threadsperblock), ceil(float(N)/threadsperblock));

    gpumatmul<<<blocks, threads>>>(A, B, C, N);
    cudaDeviceSynchronize();
}

bool isequal(int *A, int*B, int N)
{
   for(int i=0; i<N*N; i++)
   {
    if(A[i]!=B[i])
    {
        return false;
    }
   }
   return true;
}




int main()
{
    int N= 512;
    int *A, *B, *C, *D, *a, *b, *c;

    int size = N * N * sizeof(int);

    A = (int*)malloc(size);
    B = (int*)malloc(size);
    C = (int*)malloc(size);
    D = (int*)malloc(size);
    
    for(int i=0; i<N*N; i++)
    {
        A[i]= rand()%100;
        B[i] = rand()%100;
    }

    clock_t start, end;

    start = clock();
    cpumatmul(A, B, C, N);
    end = clock();

    float timeTakenCPU = ((float)(end-start)) / CLOCKS_PER_SEC ;

    cudaMalloc(&a, size);
    cudaMalloc(&b, size);
    cudaMalloc(&c, size);

    cudaMemcpy(a, A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(b, B, size, cudaMemcpyHostToDevice);

    start = clock();
    gpulauncher(a, b, c, N);
    cudaMemcpy(D, c, size, cudaMemcpyDeviceToHost);
    end = clock();
    float timeTakenGPU = ((float)(end - start)) / CLOCKS_PER_SEC;

    cudaFree(a);
    cudaFree(b);
    cudaFree(c);

    bool success = isequal(C, D, N);

    printf("Matrix Multiplication\n");
    printf("--------------------\n");
    printf("CPU Time: %f seconds\n", timeTakenCPU);
    printf("GPU Time: %f seconds\n", timeTakenGPU);
    printf("Speed Up: %f\n", timeTakenCPU / timeTakenGPU);
    printf("Verification: Matrices are %s\n", success ? "equal" : "not equal");

    free(A);
    free(B);
    free(C);
    free(D);

    return 0;
}





