EXECUTABLE := cudaSort

CU_FILES   := sort.cu

CU_DEPS    :=

CC_FILES   := main.cpp


OBJDIR=objs
CXX=g++ -m64
CXXFLAGS=-O3 -Wall

LDFLAGS=-L/usr/local/cuda-11.2/lib64/ -lcudart


NVCC=nvcc
NVCCFLAGS=-O3 -m64 #--gpu-architecture compute_35

#增加文件后要进行增加
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
