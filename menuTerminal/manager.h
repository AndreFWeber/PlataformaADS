#ifndef MANAGER
#define MANAGER

//Baudrate do MICAz = 115200
#ifndef BAUDRATE
#define BAUDRATE B115200
#endif

#define MAX_BITS_RECEPCAO 256

#define COORDENADOR_NOK -2 
#define ERRO_CONEXAO_SERIAL -1 
#define ERRO_SELECT -1 
#define ERRO_OPEN_FILE -3
#define TIMEOUT 0 
#define COORDENADOR_OK 1 
#define RECEBEU_STATUS_MOTES 2 
#define FIM 3 


#define MAX_BITS_RECEPCAO 256
#define ARQUIVO_EXPERIMENTO "plataformaDeExperimenos.txt"

enum estados{Esperando_configuracao, aguardando_status, mostrando_status};
int estado = Esperando_configuracao;
int fd; //File descriptor para a Serial USB

#endif
