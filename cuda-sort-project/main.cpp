#include <iostream>
#include <cstdio>
#include <algorithm>
#include <getopt.h>
#include <string.h>
#include <fstream>
#include "CycleTimer.h"

void usage(const char *progname)
{
    printf("Usage: %s [options]\n", progname);
    printf("Program Options:\n");
    printf("  -i  --input  <name> the name of the txt document\n");
    printf("  -?  --help         This message\n");
}

bool verifyResult(int length, int *input, int *output)
{
    int *tmp = new int[length];
    memcpy(tmp, input, length * sizeof(int));
    std::sort(tmp, tmp + length);
    for (int i = 0; i < length; i++)
        if (tmp[i] != output[i])
        {
            printf("i=%d\n", i);
            printf("output[%d]=%d\n", i, output[i]);
            printf("tmp[%d]=%d\n", i, tmp[i]);
            delete[] tmp;
            return false;
        }
    delete[] tmp;
    return true;
}

int loadinput(const std::string &filename, int **input)
{
    std::fstream infile(filename.c_str(), std::ios::in);
    if (!infile)
        exit(-1);

    // *input=new int [10000000+100];
    *input = new int[10000000 + 10];
    int length;
    for (length = 0; !infile.fail(); length++)
    {
        infile >> (*input)[length]; //每个一个空格传入一次。
    }
    infile.close();
    return length - 1;
}



void enumSort(int N,int input[],int output[]);
void mergeSort(int N,int input[],int output[]);
void printCudaInfo();


int main(int argc, char **argv)
{

    //parser args
    std::string filename = "random";

    int opt;
    static struct option long_options[] = {
        //  name, has_args,flgg,value
        {"input", 1, 0, 'i'},
        {"help", 0, 0, '?'},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "i:?", long_options, NULL)) != EOF)
    {

        switch (opt)
        {
        case 'i':
        {
            filename = (std::string)optarg;
            break;
        }
        case '?':
        default:
            usage(argv[0]);
            return 1;
        }
    }

    filename += ".txt";

    //load input
    int *input = NULL;
    int length = loadinput(filename, &input);

    //prepare output
    int *output = new int[length];
    memset(output, 0, length * sizeof(int));


    printCudaInfo();

    double minCuda_enumerationSort = 1e30;
    for (int i = 0; i < 5; ++i) {
        double startTime = CycleTimer::currentSeconds();
        enumSort(length,input,output);
        double endTime = CycleTimer::currentSeconds();
        minCuda_enumerationSort = std::min(minCuda_enumerationSort, endTime - startTime);
        if (!verifyResult(length,input,output))
        {
            printf ("Error : Sort result does not match!\n");

            delete[] output;
            delete[] input;
            exit(-1);
        }
        memset(output,0,length*sizeof(int));
    }

    //使用GPU后，1000000的enumSort比32线程的CPU快10倍。
    printf("[minCuda_enumerationSort]:\t\t[%.3f] ms\n", minCuda_enumerationSort * 1000);

    double minCuda_mergeSort = 1e30;
    for (int i = 0; i < 5; ++i) {
        double startTime = CycleTimer::currentSeconds();
        mergeSort(length,input,output);
        double endTime = CycleTimer::currentSeconds();
        minCuda_mergeSort = std::min(minCuda_mergeSort, endTime - startTime);
        if (!verifyResult(length,input,output))
        {
            printf ("Error : Sort result does not match!\n");

            delete[] output;
            delete[] input;
            exit(-1);
        }
        memset(output,0,length*sizeof(int));
    }

    printf("[minCuda_mergeSort]:\t\t[%.3f] ms\n", minCuda_mergeSort * 1000);

}