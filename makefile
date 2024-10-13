CC = g++
CFLAGS = -g -Wall -O0
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
	
debug:
	$(CHECKCC) $(CHECKFLAGS) ./a.out python_file/100cell_case.txt
	
clean:
	rm -rf src/lib/*.o main $(OUT)