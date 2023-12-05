Proyecto cliente servidor en lenguaje C para la materia de sistemas operativos

# Proyecto de Comunicación Cliente-Servidor en C

Este proyecto implementa una comunicación entre dos máquinas virtuales, un cliente y un servidor, utilizando C para la implementación del código y Python para el análisis y presentación de datos.

## Estructura del Proyecto

El proyecto está estructurado de la siguiente manera:

### Cliente
- `main.c`: Archivo principal del cliente.
- `cliente.c`: Código fuente del cliente.
- `cliente.h`: Encabezado del cliente.

### Servidor
- `main.c`: Archivo principal del servidor.
- `servidor.c`: Código fuente del servidor.
- `servidor.h`: Encabezado del servidor.

### Scripts
- `dashboard.py`: Script en Python utilizado por el servidor para generar un dashboard y notificar sobre excesos de `LOG_ERR` hacia whatsapp por medio de twillio .

## Compilación

### Cliente
Para compilar el cliente, ejecuta el siguiente comando desde el directorio del cliente:
```bash
make cliente
```

### Servidor
Para compilar el servidor, ejecuta el siguiente comando desde el directorio del servidor:
```bash
make servidor
```

### Stress
Para compilar el estrés
```bash
gcc prueba_stress.c -o stress
```
## Ejecución
Se debe ejecutar primero el servidor: 
```bash
./servidor
```
Luego ejecutamos el cliente:
```bash
./cliente
```
Para ejecutar el stress
```bash
./stress
```



