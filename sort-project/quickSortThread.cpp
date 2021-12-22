#include <stdio.h>
#include <cstring>
#include <pthread.h>
#include <iostream>
#include <cmath>
#include<unistd.h>
#include <assert.h>
// #include<sys/types.h>
#include "CycleTimer.h"

extern int partition(int data[], int l, int r);
extern void _quickSortSerial(int l, int r, int data[]);
const int P_GATE=5000;

typedef struct {
    int l;
    int r;
    int *data;
    int m;
    int threadId;
} quickWorkerArgs;


const static int MAX_THREADS = 40;
static  pthread_t workers[MAX_THREADS];
static quickWorkerArgs workerArgs[MAX_THREADS];


//实现并行之后速度没有提升。
void *quickThreadStart(void* threadArgs)
{
    quickWorkerArgs* args = static_cast<quickWorkerArgs*>(threadArgs);
    
    assert(args->m>=0);
    if (((args->r-args->l)<P_GATE )|| args->m==0)
    {
        // printf("id=%d\n",args->threadId);
        // printf("left=%d,right=%d\n",args->l,args->r);
        _quickSortSerial(args->l,args->r,args->data);
        return NULL;
    }
    int k=partition(args->data,args->l,args->r);

    int son_id=args->threadId+(int)pow(2,args->m-1);
    workerArgs[son_id].data=args->data;
    workerArgs[son_id].l=k+1;
    workerArgs[son_id].r=args->r;
    workerArgs[son_id].threadId=son_id;
    workerArgs[son_id].m=args->m-1;


    workerArgs[args->threadId].l=args->l;
    workerArgs[args->threadId].r=k;
    workerArgs[args->threadId].m=args->m-1;
    workerArgs[args->threadId].threadId=args->threadId;    

    if (pthread_create(&workers[son_id],0,quickThreadStart,&workerArgs[son_id])!=0)
    {
        exit(-1);
    }
    quickThreadStart(&workerArgs[args->threadId]);
    return NULL;
}


void quickSortThread(int numThreads, int l, int r, int input[], int output[])
{
    memcpy(output, input, (r - l) * sizeof(int));

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(-1);
    }
    int m=(int)log2(numThreads);
    workerArgs[0].data=output;workerArgs[0].l=l;workerArgs[0].r=r;workerArgs->threadId=0;workerArgs->m=m;
    // quickThreadStart(&workerArgs[0]);
    pthread_create(&workers[0],NULL,quickThreadStart,&workerArgs[0]);

    for (int i=0; i<numThreads; i++)
        pthread_join(workers[i], NULL);
    return;
}