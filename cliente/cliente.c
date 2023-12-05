#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h> // Para usar alarm y SIGALRM
#include <errno.h>

#define PUERTO 8080
#define TAM_BUFFER 1024
// Variable global para verificar si ha pasado el tiempo de monitoreo
volatile sig_atomic_t tiempo_cumplido = 0;
pid_t pid;
// Función de manejo de la señal de alarma
void handle_alarm(int signum) {
    tiempo_cumplido = 1;
    printf("¡Señal de alarma recibida! Tiempo cumplido.\n");
     kill(pid, SIGTERM); // Primero intentamos enviar SIGTERM
    sleep(1); // Espera breve para dar tiempo al proceso para responder a SIGTERM

    // Si SIGTERM no funcionó, intentamos con SIGKILL
    kill(pid, SIGKILL); // Enviar SIGKILL para forzar la terminación
   // Cerrar el socket u otros recursos

    exit(EXIT_SUCCESS); // Finalizar el programa
     
}
void iniciarCliente() {
    int sockfd;
    char servicio1[] = "sshd";
    char servicio2[] = "apache2";
    int tiempoActualizacion = 60;
    struct sockaddr_in servidor_addr;
    // Establecer la señal SIGALRM para manejar el tiempo de monitoreo
    signal(SIGALRM, handle_alarm);

    // Establecer el tiempo de monitoreo usando alarm()
    alarm(tiempoActualizacion);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_port = htons(PUERTO);
    servidor_addr.sin_addr.s_addr = inet_addr("192.168.100.128");

    if (connect(sockfd, (struct sockaddr *)&servidor_addr, sizeof(servidor_addr)) == -1) {
        perror("Error al conectar");
        exit(EXIT_FAILURE);
    }

    // Crear un pipe para la comunicación entre procesos
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Error al crear el pipe");
        exit(EXIT_FAILURE);
    }

     pid = fork();
    if (pid == -1) {
        perror("Error al crear el proceso hijo");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Proceso hijo
        close(pipefd[0]);  // Cerrar extremo de lectura del pipe en el proceso hijo
        dup2(pipefd[1], STDOUT_FILENO);  // Redirigir la salida estándar al pipe
        close(pipefd[1]);  // Cerrar extremo de escritura del pipe en el proceso hijo

        // Ejecutar journalctl en el proceso hijo
        ejecutarJournalctl(servicio1, servicio2, tiempoActualizacion);
    } else {  // Proceso padre
            ssize_t bytes_leidos;
            char buffer[TAM_BUFFER];
            char linea[TAM_BUFFER];
            int pos = 0;

        while (!tiempo_cumplido) {
            
            bytes_leidos = read(pipefd[0], buffer, TAM_BUFFER);
            if (bytes_leidos > 0) {
                for (int i = 0; i < bytes_leidos; ++i) {
                    linea[pos++] = buffer[i];
                    if (buffer[i] == '\n') {
                        linea[pos] = '\0'; // Agregar terminador nulo para indicar el final de la línea
                        ssize_t enviado = send(sockfd, linea, strlen(linea), 0);
                        if (enviado == -1) {
                            perror("Error al enviar al servidor");
                            exit(EXIT_FAILURE);
                        }
                        printf("Mensaje enviado al servidor: %s", linea);
                        pos = 0; // Reiniciar posición para la siguiente línea
                    }
                }
            } else if (bytes_leidos == -1) {
                perror("Error al leer del pipe");
                exit(EXIT_FAILURE);
            }
            memset(buffer, 0, TAM_BUFFER);    
        }
        if (pos > 0) {
            linea[pos] = '\0'; // Agregar terminador nulo para indicar el final de la línea
            ssize_t enviado = send(sockfd, linea, strlen(linea), 0);
            if (enviado == -1) {
                perror("Error al enviar al servidor");
                exit(EXIT_FAILURE);
            }
            printf("Mensaje enviado al servidor: %s", linea);
        }
        close(pipefd[1]);  // Cerrar extremo de escritura del pipe en el proceso padre
        kill(pid, SIGTERM); // Terminar el proceso hijo
        close(pipefd[0]);  // Cerrar extremo de lectura del pipe en el proceso padre
        close(sockfd);
        wait(NULL);  // Esperar a que el proceso hijo termine
        exit(EXIT_SUCCESS);
    }
}

void ejecutarJournalctl(char *servicio1, char *servicio2, int tiempoActualizacion) {
    char comando[100];  // Longitud suficiente para contener el comando

    snprintf(comando, sizeof(comando), "journalctl -t %s -t %s -f ", servicio1, servicio2, tiempoActualizacion);
    char *args[] = { "/bin/sh", "-c", comando, NULL };

    if (execvp(args[0], args) == -1) {
        perror("Error al ejecutar execvp");
        exit(EXIT_FAILURE);
    }
}

