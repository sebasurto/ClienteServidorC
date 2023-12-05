# Makefile para compilar el servidor

CC = gcc
CFLAGS = -Wall -Wextra -pthread # Flags de compilación, incluyendo -pthread para pthreads

servidor: servidor.o main.o
	$(CC) $(CFLAGS) $^ -o $@

servidor.o: servidor.c
	$(CC) $(CFLAGS) -c $<

main.o: main.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o servidor
