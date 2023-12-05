#ifndef SERVIDOR_H
#define SERVIDOR_H

int iniciar_servidor();
void sigintHandler(int sig_num);
void *threadFunc(void *arg);
#endif
