CC = g++
FLAGS = -g -std=c++11 -O3

SOURCES = *.h *.cpp

EXENAME = hashmap

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(SOURCES)

clean:
	rm -f *.o $(EXENAME)

.PHONY: all clean
