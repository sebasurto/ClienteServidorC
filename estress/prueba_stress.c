#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

int main() {
    openlog("sshd", LOG_PID | LOG_NDELAY, LOG_USER);

    // Generar 5 registros para el servicio ssh con diferentes prioridades
    for (int i = 11; i < 15; ++i) {
        syslog(LOG_AUTH, "Registro %d para SSH con prioridad LOG_AUTH", i + 1);
        syslog(LOG_INFO, "Registro %d para SSH con prioridad LOG_INFO", i + 1);
        syslog(LOG_DEBUG, "Registro %d para SSH con prioridad LOG_DEBUG", i + 1);
    }

    closelog();

    openlog("apache2", LOG_PID | LOG_NDELAY, LOG_USER);

    // Generar 5 registros para el servicio apache2 con diferentes prioridades
    for (int i = 11; i < 15; ++i) {
        syslog(LOG_DAEMON, "Registro %d para Apache2 con prioridad LOG_DAEMON", i + 1);
        syslog(LOG_NOTICE, "Registro %d para Apache2 con prioridad LOG_NOTICE", i + 1);
        syslog(LOG_ERR, "Registro %d para Apache2 con prioridad LOG_ERR", i + 1);
    }

    closelog();

    return 0;
}

