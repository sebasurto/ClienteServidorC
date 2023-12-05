#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "servidor.h"
#include <stdbool.h>

#define PUERTO 8080
#define TAM_BUFFER 1024

int servidor_fd, nuevo_socket;
pthread_mutex_t mutex_archivo;


void *threadFunc(void *arg) {
    int nuevo_socket = *((int *)arg);
    char buffer[TAM_BUFFER] = {0};
    ssize_t bytes_leidos;
    char command[1000];
    // Leer los datos recibidos del cliente
    while ((bytes_leidos = read(nuevo_socket, buffer, TAM_BUFFER)) > 0) {
        pthread_mutex_lock(&mutex_archivo);

        // Almacenar los datos en un archivo de texto
        FILE *archivo = fopen("datos.txt", "a");
        if (archivo == NULL) {
            perror("Error al abrir el archivo");
            exit(EXIT_FAILURE);
        }
        fprintf(archivo, "%s\n", buffer);
        fclose(archivo);

        
        pthread_mutex_unlock(&mutex_archivo);
        memset(buffer, 0, TAM_BUFFER); // Limpiar el buffer para nuevos datos
        // Comando para ejecutar el script de Python
        sprintf(command, "python3 dashboard.py");

         // Ejecutar el comando usando system
        int status = system(command);
        // Liberar el acceso al archivo
    }

    if (bytes_leidos == 0) {
        printf("El cliente cerró la conexión\n");
    } else {
        perror("Error al leer del cliente");
    }

    close(nuevo_socket);
    free(arg); // Liberar memoria asignada para el descriptor de socket
    return NULL;
}

void sigintHandler(int sig_num) {
    printf("\nSeñal SIGINT (Ctrl+C) detectada. Cerrando el servidor...\n");

    close(servidor_fd);
    exit(EXIT_SUCCESS);
}

int iniciar_servidor() {
    struct sockaddr_in direccion_servidor, direccion_cliente;
    int addrlen = sizeof(direccion_cliente);
    pthread_t tid;
    
    // Configurar el manejador de señales para SIGINT (Ctrl+C)
    signal(SIGINT, sigintHandler);

    // Crear socket
    if ((servidor_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = INADDR_ANY;
    direccion_servidor.sin_port = htons(PUERTO);

    // Asignar una dirección y un puerto al socket
    if (bind(servidor_fd, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) == -1) {
        perror("Error en bind");
        exit(EXIT_FAILURE);
    }

    // Escuchar por conexiones entrantes
    if (listen(servidor_fd, 3) == -1) {
        perror("Error en listen");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&mutex_archivo, NULL) != 0) {
        perror("Error al inicializar el mutex");
        exit(EXIT_FAILURE);
    }
    while (1) {
        int *nuevo_socket = malloc(sizeof(int));
        *nuevo_socket = accept(servidor_fd, (struct sockaddr *)&direccion_cliente, (socklen_t*)&addrlen);
        if (*nuevo_socket == -1) {
            perror("Error en accept");
            exit(EXIT_FAILURE);
        }
        
        // Crear un hilo para manejar la comunicación con el cliente
        if (pthread_create(&tid, NULL, threadFunc, (void *)nuevo_socket) != 0) {
            perror("Error al crear el hilo");
            exit(EXIT_FAILURE);
        }
        
        // Liberar el hilo cuando finalice su ejecución
        pthread_detach(tid);
        

    }
    pthread_mutex_destroy(&mutex_archivo);
    return 0;
}

