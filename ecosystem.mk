CC = g++
CFLAGS = -fopenmp
SRCS = eco_par.cpp
EXEC = ecosystem

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(EXEC)

clean:
	rm -f $(EXEC)