void enumSortSerial(int begin,int end,int length,int input[],int output[])
{
    for (int i=begin;i<end;i++)
    {
        int k=0;
        for (int j=0;j<length;j++)
        {
            if (input[i]>input[j])
            {
                k++;
            }
        }
        output[k]=input[i];
    }
    return ;
}