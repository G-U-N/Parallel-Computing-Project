#include <cstring>
#include <iostream>
#include <cmath>


typedef struct {
    int l;
    int r;
    int *data;
    int m;
    int threadId;
} mergeWorkerArgs;

const int MAX_THREADS=40;
const int P_Gate=1000;
static  pthread_t workers[MAX_THREADS];
static mergeWorkerArgs workerArgs[MAX_THREADS];


extern void mergeSerialHelper(int l,int r,int data[]);
extern void merge(int l,int m,int r,int data[]);
void *mergeThreadStart(void *threadArgs)
{
    mergeWorkerArgs* args = static_cast<mergeWorkerArgs*>(threadArgs);
    if ((args->r-args->l)<P_Gate || args->m==0)
    {
        mergeSerialHelper(args->l,args->r,args->data);
        return NULL;
    }
    int l=args->l;int r=args->r;
    int k=(int)((l+r)/2);
    int son_id=args->threadId+(int)pow(2,args->m-1);
    workerArgs[son_id].threadId=son_id;
    workerArgs[son_id].l=k;
    workerArgs[son_id].r=r;
    workerArgs[son_id].data=args->data;
    workerArgs[son_id].m=args->m-1;

    pthread_create(&workers[son_id],NULL,mergeThreadStart,&workerArgs[son_id]);


    workerArgs[args->threadId].m=args->m-1;
    workerArgs[args->threadId].r=k;
    mergeThreadStart(&workerArgs[args->threadId]);

    pthread_join(workers[son_id],NULL);

    merge(l,k,r,args->data);
    return NULL;
}


void mergeSortThread(int numThreads, int l, int r, int input[], int output[])
{
    memcpy(output, input, (r - l) * sizeof(int));

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(-1);
    }
    int m=(int)log2(numThreads);
    workerArgs[0].data=output;workerArgs[0].l=l;workerArgs[0].r=r;workerArgs->threadId=0;workerArgs->m=m;
    mergeThreadStart(&workerArgs[0]);

    return;
}