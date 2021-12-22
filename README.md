程序运行方式


一、**编译程序**，进入sort-project(cuda-sort-project)，输入命令行

```shell
make
```

程序即可自动编译为可以执行文件sort(cudaSort)。

二、**运行可执行程序**，输入命令行

```shell
./sort 或 ./cudaSort
```

三、**删除程序**

```shell
make clean
```

四、**指定线程数量和需要排序的数据集**

```shell
./sort -t threads_num -i document_name
```

其中threads_num表示指明的线程数量(default=2), document_name为需要读取的数据集名(default="random.txt")

例如32个线程运行1000000.txt文件，输入命令

```shell
./sort -t 32 -i 1000000
```

> 注意这里文件名不要加入后缀



五、**编译与运行cuda-sort-project的注意事项**

- 请确保设备拥有GPU且与之匹配的CUDA。
- cuda-sort-project的Makefile文件如下

```Makefile
EXECUTABLE := cudaSort

CU_FILES   := sort.cu

CU_DEPS    :=

CC_FILES   := main.cpp


OBJDIR=objs
CXX=g++ -m64
CXXFLAGS=-O3 -Wall

LDFLAGS=-L/usr/local/cuda-11.2/lib64/ -lcudart

#如果需要运行，需要将此处的LDFLAGS改为-L (YOUR_PATH_OF_CUDA_LIB) -lcudart。需要将其链接到自己设备的cuda的位置。


NVCC=nvcc
NVCCFLAGS=-O3 -m64 #--gpu-architecture compute_35

OBJS=$(OBJDIR)/main.o  $(OBJDIR)/sort.o


.PHONY: dirs clean

default: $(EXECUTABLE)

dirs:
		mkdir -p $(OBJDIR)/

clean:
		rm -rf $(OBJDIR) *.ppm *~ $(EXECUTABLE)

#对生成的.o文件进行链接。由于依赖于OBJS，因此会执行OBJS的指令。
$(EXECUTABLE): dirs $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
		$(CXX) $< $(CXXFLAGS) -c -o $@


$(OBJDIR)/%.o: %.cu
		$(NVCC) $< $(NVCCFLAGS) -c -o $@
```
