all:
	g++ -std=c++17 -g main.cpp app_main.c -lX11 -lpthread -o guibench

clean:
	rm -f a.out guibench *.o *.core
