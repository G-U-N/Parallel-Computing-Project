#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>
#include <assert.h>

#include "CycleTimer.h"

float toBW(int bytes, float sec) {
  return static_cast<float>(bytes) / (1024. * 1024. * 1024.) / sec;
}


__global__ void
enum_kernel(int N, int input[], int output[]) {



    //每个kernel用来确定一个input所在的位置。
    int index = blockIdx.x * blockDim.x + threadIdx.x;

    if (index < N)
    {
        int k=0;
        for (int j=0;j<N;j++)
        {
            if (input[index]>input[j])
            {
                k++;
            }
        }
        output[k]=input[index];
    }        
}

__device__ void merge(int l,int m,int r,int data[],int tmp[])
{
    int i=l,j=m,k=l;
    while (i<m&&j<r)
    {
        if (tmp[i]<=tmp[j])
        {
            data[k++]=tmp[i++];
        }
        else
        {
            data[k++]=tmp[j++];
        }
    }
    while (i<m) data[k++]=tmp[i++];
    while (j<r) data[k++]=tmp[j++];
}



__global__ void
merge_kernel(int N, int chunk,int data[],int tmp[]) 
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;

    if (index>N) return ;
    //当数据量过多时，这里会发生溢出。暂时使用变负数。
    int start=index*chunk;
    if (start>=N || start<0) return ;

    int left=start;
    int mid=min(start+(int)(chunk/2),N);
    int right=min(start+chunk,N);
    // printf("l=%d,m=%d,r=%d\n",left,mid,right);
    // if (start<0) assert(0);
    merge(left, mid,right,data,tmp);
}

void enumSort(int N,int input[],int output[])
{
    cudaSetDevice(1);
    int totalBytes = sizeof(int)* 2*N;
    const int threadsPerBlock = 512;
    const int blocks = (N + threadsPerBlock - 1) / threadsPerBlock;

    int *device_i;
    int *device_o;

    cudaMalloc((void **)&device_i, N*sizeof(int));
    cudaMalloc((void **)&device_o, N*sizeof(int));

    double startTime = CycleTimer::currentSeconds();

    cudaMemcpy(device_i, input, N*sizeof(int), cudaMemcpyHostToDevice);

    double startKernelTime = CycleTimer::currentSeconds();

    enum_kernel<<<blocks,threadsPerBlock>>>(N,device_i,device_o);    

    // cudaThreadSynchronize();
    cudaDeviceSynchronize();
    double endKernelTime = CycleTimer::currentSeconds();
    cudaMemcpy(output, device_o, N*sizeof(int), cudaMemcpyDeviceToHost);
    double endTime = CycleTimer::currentSeconds();

    cudaError_t errCode = cudaPeekAtLastError();
    if (errCode != cudaSuccess) {
        fprintf(stderr, "WARNING: A CUDA error occured: code=%d, %s\n", errCode, cudaGetErrorString(errCode));
    }

    double overallDuration = endTime - startTime;
    double overallComputeDuration = endKernelTime - startKernelTime;
    printf("Comput time: %.3f ms\n", 1000.f * overallComputeDuration);
    printf("Overall time: %.3f ms\t\t[%.3f GB/s]\n", 1000.f * overallDuration, toBW(totalBytes, overallDuration));

    cudaFree(device_i);
    cudaFree(device_o);
}


//cuda merge sort 就是chunk取2的幂次然后每个kernel会出里start+chunk的数据



void mergeSort(int N,int input[],int output[])
{
    cudaSetDevice(1);
    int totalBytes = sizeof(int) * 3*N;
    // const int blocks = (((N + threadsPerBlock - 1) / threadsPerBlock))/2+1;

    int *device_i;
    int *tmp;
    cudaMalloc((void **)&device_i, N*sizeof(int));
    cudaMalloc((void **)&tmp, N*sizeof(int));

    double startTime = CycleTimer::currentSeconds();

    cudaMemcpy(device_i, input, N*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(tmp, input, N*sizeof(int), cudaMemcpyHostToDevice);

    double startKernelTime = CycleTimer::currentSeconds();

    
    for (int chunk=2;chunk<2*N;chunk*=2)
    {
        // const int threadsPerBlock = 512;
        const int threadsPerBlock=1;
        const int blocks = ((N + threadsPerBlock*chunk - 1) / (threadsPerBlock*chunk));
        merge_kernel<<<blocks,threadsPerBlock>>>(N,chunk,device_i,tmp);
        cudaDeviceSynchronize();
        merge_kernel<<<blocks,threadsPerBlock>>>(N,chunk,tmp,device_i);
        cudaDeviceSynchronize();
    }
    

    double endKernelTime = CycleTimer::currentSeconds();
    cudaMemcpy(output, device_i, N*sizeof(int), cudaMemcpyDeviceToHost);
    double endTime = CycleTimer::currentSeconds();

    cudaError_t errCode = cudaPeekAtLastError();
    if (errCode != cudaSuccess) {
        fprintf(stderr, "WARNING: A CUDA error occured: code=%d, %s\n", errCode, cudaGetErrorString(errCode));
    }

    double overallDuration = endTime - startTime;
    double overallComputeDuration = endKernelTime - startKernelTime;
    printf("Comput time: %.3f ms\n", 1000.f * overallComputeDuration);
    printf("Overall time: %.3f ms\t\t[%.3f GB/s]\n", 1000.f * overallDuration, toBW(totalBytes, overallDuration));

    cudaFree(device_i);
    cudaFree(tmp);
}






void
printCudaInfo() {

    // for fun, just print out some stats on the machine

    int deviceCount = 0;
    cudaError_t err = cudaGetDeviceCount(&deviceCount);

    printf("---------------------------------------------------------\n");
    printf("Found %d CUDA devices\n", deviceCount);

    for (int i=0; i<deviceCount; i++) {
        cudaDeviceProp deviceProps;
        cudaGetDeviceProperties(&deviceProps, i);
        printf("Device %d: %s\n", i, deviceProps.name);
        printf("   SMs:        %d\n", deviceProps.multiProcessorCount);
        printf("   Global mem: %.0f MB\n",
               static_cast<float>(deviceProps.totalGlobalMem) / (1024 * 1024));
        printf("   CUDA Cap:   %d.%d\n", deviceProps.major, deviceProps.minor);
    }
    printf("---------------------------------------------------------\n");
}