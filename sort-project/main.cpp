#include <iostream>
#include <cstdio>
#include <algorithm>
#include <getopt.h>
#include <string.h>
#include <fstream>
#include "CycleTimer.h"

extern void enumSortSerial(int begin, int end, int length, int input[], int output[]);
extern void enumSortThread(int numThreads, int length, int input[], int output[]);
extern void quickSortSerial(int l, int r, int input[], int output[]);
extern void quickSortThread(int numThreads, int l, int r, int input[], int output[]);
extern void mergeSortSerial(int l, int r, int input[], int output[]);
extern void mergeSortThread(int numThreads, int l, int r, int input[], int output[]);

void usage(const char *progname)
{
    printf("Usage: %s [options]\n", progname);
    printf("Program Options:\n");
    printf("  -t  --threads <N>  Use N threads\n");
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
    *input = new int[1000000 + 10];
    int length;
    for (length = 0; !infile.fail(); length++)
    {
        infile >> (*input)[length]; //每个一个空格传入一次。
    }
    infile.close();
    return length - 1;
}

int main(int argc, char **argv)
{

    //parser args
    int numThreads = 2;
    std::string filename = "random";

    int opt;
    static struct option long_options[] = {
        //  name, has_args,flgg,value
        {"threads", 1, 0, 't'},
        {"input", 1, 0, 'i'},
        {"help", 0, 0, '?'},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "t:i:?", long_options, NULL)) != EOF)
    {

        switch (opt)
        {
        case 't':
        {
            numThreads = atoi(optarg);
            break;
        }
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

    //test
    double minSerial_enumerationSort = 1e30;
    for (int i = 0; i < 5; ++i) {
        double startTime = CycleTimer::currentSeconds();
        //my sort function;
        enumSortSerial(0,length,length,input,output);
        double endTime = CycleTimer::currentSeconds();
        minSerial_enumerationSort = std::min(minSerial_enumerationSort, endTime - startTime);
        if (!verifyResult(length,input,output))
        {
            printf ("Error : Sort result does not match!\n");

            delete[] output;
            delete[] input;
            exit(-1);
        }
        memset(output,0,length*sizeof(int));
    }

    printf("[minSerial_enumerationSort]:\t\t[%.3f] ms\n", minSerial_enumerationSort * 1000);

    double minThread_enumerationSort = 1e30;
    for (int i = 0; i < 5; ++i) {
        double startTime = CycleTimer::currentSeconds();
        enumSortThread(numThreads,length,input,output);
        double endTime = CycleTimer::currentSeconds();
        minThread_enumerationSort = std::min(minThread_enumerationSort, endTime - startTime);
        if (!verifyResult(length,input,output))
        {
            printf ("Error : Sort result does not match!\n");

            delete[] output;
            delete[] input;
            exit(-1);
        }
        memset(output,0,length*sizeof(int));
    }

    printf("[minThread_enumerationSort]:\t\t[%.3f] ms\n", minThread_enumerationSort * 1000);

    printf("\t\t\t\t(%.2fx speedup from %d threads)\n", minSerial_enumerationSort/minThread_enumerationSort, numThreads);

    double minSerial_quickSort = 1e30;
    for (int i = 0; i < 5; ++i) {
        double startTime = CycleTimer::currentSeconds();
        //my sort function;
        quickSortSerial(0,length,input,output);
        double endTime = CycleTimer::currentSeconds();
        minSerial_quickSort = std::min(minSerial_quickSort, endTime - startTime);
        if (!verifyResult(length,input,output))
        {
            printf ("Error : Sort result does not match!\n");

            delete[] output;
            delete[] input;
            exit(-1);
        }
        memset(output,0,length*sizeof(int));
    }

    printf("[minSerial_quickSort]:\t\t[%.3f] ms\n", minSerial_quickSort * 1000);

    double minThread_quickSort = 1e30;
    for (int i = 0; i < 5; ++i) {
        double startTime = CycleTimer::currentSeconds();
        quickSortThread(numThreads,0,length,input,output);
        double endTime = CycleTimer::currentSeconds();
        minThread_quickSort = std::min(minThread_quickSort, endTime - startTime);
        if (!verifyResult(length,input,output))
        {
            printf ("Error : Sort result does not match!\n");

            delete[] output;
            delete[] input;
            exit(-1);
        }
        memset(output,0,length*sizeof(int));
    }

    printf("[minThread_quickSort]:\t\t[%.3f] ms\n", minThread_quickSort * 1000);

    printf("\t\t\t\t(%.2fx speedup from %d threads)\n", minSerial_quickSort/minThread_quickSort, numThreads);

    double minSerial_mergeSort = 1e30;
    for (int i = 0; i < 5; ++i)
    {
        double startTime = CycleTimer::currentSeconds();
        //my sort function;
        mergeSortSerial(0, length, input, output);
        double endTime = CycleTimer::currentSeconds();
        minSerial_mergeSort = std::min(minSerial_mergeSort, endTime - startTime);
        if (!verifyResult(length, input, output))
        {
            printf("Error : Sort result does not match!\n");

            delete[] output;
            delete[] input;
            exit(-1);
        }
        memset(output, 0, length * sizeof(int));
    }

    printf("[minSerial_mergeSort]:\t\t[%.3f] ms\n", minSerial_mergeSort * 1000);

    double minThread_mergeSort = 1e30;
    for (int i = 0; i < 5; ++i)
    {
        double startTime = CycleTimer::currentSeconds();
        mergeSortThread(numThreads, 0, length, input, output);
        double endTime = CycleTimer::currentSeconds();
        minThread_mergeSort = std::min(minThread_mergeSort, endTime - startTime);
        if (!verifyResult(length, input, output))
        {
            printf("Error : Sort result does not match!\n");

            delete[] output;
            delete[] input;
            exit(-1);
        }
        memset(output, 0, length * sizeof(int));
    }

    printf("[minThread_mergeSort]:\t\t[%.3f] ms\n", minThread_mergeSort * 1000);

    printf("\t\t\t\t(%.2fx speedup from %d threads)\n", minSerial_mergeSort / minThread_mergeSort, numThreads);

    delete[] input;
    delete[] output;

    return 0;
}