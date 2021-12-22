#include <cstring>
#include <cstdio>


void merge(int l,int m,int r,int data[])
{
    //拷贝
    int n1=m-l,n2=r-m;
    int *ll=new int[n1];
    int *rr=new int[n2];

    for (int i=0;i<n1;i++)
    {
        ll[i]=data[i+l];
    }
    for (int j=0;j<n2;j++)
    {
        rr[j]=data[j+m];
    }

    //合并
    int i=0,j=0;
    int k=l;

    while(i<n1&&j<n2)
    {
        if (ll[i]<=rr[j])
        {
            data[k++]=ll[i++];
        }
        else
        {
            data[k++]=rr[j++];
        }
    }

    while (i<n1)
    {
        data[k++]=ll[i++];
    }
    while (j<n2)
    {
        data[k++]=rr[j++];
    }

    delete []ll;
    delete []rr;

}

//左闭右开
void mergeSerialHelper(int l,int r,int data[])
{   
    // printf("l=%d,r=%d\n",l,r);
    if (l>=r-1) return ;
    int m=(int)((l+r)/2);

    mergeSerialHelper(l,m,data);
    mergeSerialHelper(m,r,data);

    merge(l,m,r,data);


}
void mergeSortSerial(int l,int r,int input[],int output[])
{
    memcpy(output,input,sizeof(int)*(r-l));
    mergeSerialHelper(l,r,output);
}