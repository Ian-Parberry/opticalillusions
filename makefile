all: main.cpp
	g++ -o main.exe -std=c++11 main.cpp

cleanup:
	rm -f .makefile.*

