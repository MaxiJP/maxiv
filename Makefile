all: gcc

CC = gcc

gcc:
	$(CC) -o maxiv maxiv.c -I/usr/local/include/SDL2 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL2
