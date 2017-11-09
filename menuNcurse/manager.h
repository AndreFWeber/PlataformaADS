#ifndef MANAGER
#define MANAGER

//Baudrate do MICAz = 115200
#ifndef BAUDRATE
#define BAUDRATE B115200
#endif

#define ERRO_CONEXAO_SERIAL -1 
#define COORDENADOR_NOK -2 
#define COORDENADOR_OK 1 
#define ERRO_SELECT -1 
#define TIMEOUT 0 
#define MAX_BITS_RECEPCAO 256

enum estados{Esperando_configuracao, aguardando_status, mostrando_status};
int estado = Esperando_configuracao;
int fd; //File descriptor para a Serial USB

#endif
