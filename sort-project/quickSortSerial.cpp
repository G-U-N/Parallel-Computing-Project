#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>

void swap(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;
}

int partition(int data[], int l, int r)
{
    int pivot=data[r-1];
    int k=l-1;
    for (int i=l;i<r-1;i++)
    {
        if (data[i]<=pivot)
        {
            k++;
            swap(data[k],data[i]); 
        }
    }
    swap(data[k+1],data[r-1]);
    return k+1;

}

void _quickSortSerial(int l, int r, int data[])
{
    // printf("id=%d\n",getpid());
    if (l < r-1)
    {
        int k = partition(data, l, r);
        _quickSortSerial(l,k,data);
        _quickSortSerial(k + 1, r, data);
    }
    return;
}

//左闭右开
void quickSortSerial(int l, int r, int input[], int output[])
{
    memcpy(output, input, (r - l) * sizeof(int));
    _quickSortSerial(l, r, output);
    return;
}