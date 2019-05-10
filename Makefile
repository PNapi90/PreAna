CC      = g++
CFLAGS  = -g -pthread -O3 -std=c++11 -Wall
LIBFLAGS = -I ~/eigen_dir/

OBJECTS = ConeChecker.o Analyzer.o DataVault.o GammaTracker.o DeltaX.o 

all: Program

%.o: %.cpp
	$(CC) -c $< $(CFLAGS) $(LIBFLAGS)
	
Program: $(OBJECTS) Main.o
	$(CC) $(OBJECTS) $(LIBFLAGS) Main.o -pthread -o Analysis

clean: 
	rm -f *.o $(PROGRAM)
	rm Analysis
	
