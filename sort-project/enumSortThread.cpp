#include <stdio.h>
#include <pthread.h>

#include "CycleTimer.h"

typedef struct {
    int length;
    int *input;
    int *output;
    int threadId;
    int numThreads;
} enumWorkerArgs;

extern void enumSortSerial(int begin,int end,int length,int input[],int output[]);

void* enumThreadStart(void* threadArgs)
{
    enumWorkerArgs* args = static_cast<enumWorkerArgs*>(threadArgs);
    int len=(int)((args->length+args->numThreads-1)/args->numThreads);
    int begin=len*args->threadId;
    int end=(begin+len)<args->length?(begin+len):args->length;
    enumSortSerial(begin,end,args->length,args->input,args->output);
    return NULL;
}

void enumSortThread(int numThreads,int length,int input[],int output[])
{
    const static int MAX_THREADS = 40;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(-1);
    }

    pthread_t workers[MAX_THREADS];
    enumWorkerArgs args[MAX_THREADS];
    
    for (int i=0;i<numThreads;i++)
    {
        args[i].threadId=i;
        args[i].numThreads=numThreads;
        args[i].length=length;
        args[i].input=input;
        args[i].output=output;
    }

    for (int i=1; i<numThreads; i++)
        pthread_create(&workers[i], NULL, enumThreadStart, &args[i]);

    //主线程运行。
    enumThreadStart(&args[0]);

    // wait for worker threads to complete
    for (int i=1; i<numThreads; i++)
        pthread_join(workers[i], NULL);
}