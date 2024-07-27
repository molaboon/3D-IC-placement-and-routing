CC = g++
CFLAGS = -g -Wall -O3
BINS = a.out
SRC_DIRS := src/lib
OUT = test.svg placement.txt *.out cost.svg

CHECKCC = valgrind
CHECKFLAGS = --leak-check=full -s --show-leak-kinds=all --track-origins=yes 

SRCS := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS := $(SRCS:.cpp=.o)

all: $(BINS)

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c  $< -o $@

$(BINS): src/main.cpp $(OBJS)
	$(CC) $(CFLAGS) -fopenmp  $^ -o $@ -lm

do1:
	time ./main Benchmark/case1.txt case1.out
	case1.txt case1.out
	
do2:
	time ./main Benchmark/case2.txt case2.out
	case2.txt case2.out
	
do3:
	time ./main Benchmark/case3.txt case3.out
	case3.txt case3.out
	
do4:
	time ./main Benchmark/case4.txt case4.out
	case4.txt case4.out
	
debug:
	$(CHECKCC) $(CHECKFLAGS) ./main Benchmark/case2.txt case2.out
	
clean:
	rm -rf src/lib/*.o main $(OUT)