cliente: cliente.o main.o
	gcc cliente.o main.o -o cliente

cliente.o: cliente.c
	gcc -c cliente.c -o cliente.o

main_cliente.o: main.c
	gcc -c main.c -o main.o

clean:
	rm -f cliente.o main.o cliente
