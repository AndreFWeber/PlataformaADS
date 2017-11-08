#ifndef MANAGER
#define MANAGER

enum estados{Esperando_configuracao, aguardando_status, mostrando_status};
int estado = Esperando_configuracao;
int fd; //File descriptor para a Serial USB
#endif
