import sys
from twilio.rest import Client

def mensaje(mensajes):
 account_sid = ''
 auth_token = ''
 client = Client(account_sid, auth_token)

 message = client.messages.create(
  from_='whatsapp:',
  body=mensajes,
  to='whatsapp:'
 )

 #print(message.sid)

def generar_dashboard(archivo):
    
    # Abrir y leer el archivo línea por línea
    registros = {}

    # Abrir y leer el archivo línea por línea
    with open(archivo, 'r') as file:
        for linea in file:
            # Obtener el nombre del servicio y el tipo de log
            palabras = linea.split()
            
            # Verificar si hay suficientes palabras en la línea
            if len(palabras) >= 4:
                servicio = palabras[4].split('[')[0]  # Obtener el nombre del servicio antes del '['
                log = [palabra for palabra in palabras if 'LOG_' in palabra]
                
                if not log:
                    log = 'Unknwed'
                else:
                    log = log[0]

                # Actualizar el recuento en el diccionario
                if servicio not in registros:
                    registros[servicio] = {}
                
                registros[servicio][log] = registros[servicio].get(log, 0) + 1
            

    return registros

def imprimir_dashboard(registros):
    tipos_logs = set()
    for servicio, logs in registros.items():
        tipos_logs.update(logs.keys())

    tipos_logs = sorted(list(tipos_logs))

    # Imprimir el encabezado
    encabezado = "Servicio | " + " | ".join(tipos_logs)
    print(encabezado)

    # Imprimir el dashboard
    for servicio, logs in registros.items():
        fila = servicio.ljust(9)
        for log in tipos_logs:
            cantidad = logs.get(log, 0)
            fila += f"| {str(cantidad).center(len(log))} "
        print(fila)
        if 'LOG_ERR' in logs and logs['LOG_ERR'] > 50:
            mensaje(f"Hay más de 50 LOG_ERR en el servicio {servicio}")

# Ejemplo de uso
dashboard = generar_dashboard('datos.txt')
imprimir_dashboard(dashboard)

