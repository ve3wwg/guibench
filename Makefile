all:
	g++ -std=c++17 -g -Wall -pedantic-errors main.cpp app_main.c -lX11 -lpthread -o guibench

clean:
	rm -f a.out guibench *.o *.core
