CXX=g++ -m64
CXXFLAGS=-O3 -Wall -std=c++11 


APP_NAME=sort

default: $(APP_NAME)


.PHONY: clean

clean:
		rm -rf $(APP_NAME)

$(APP_NAME): 
		$(CXX) $(CXXFLAGS) *.cpp -o $@ -lm -lpthread