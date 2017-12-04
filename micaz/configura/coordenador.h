#ifndef COORDENADOR
#define COORDENADOR

#define NUM_MAX_MOTES 256 //ATENÇAO!!! Se for modificar este valor, modifique a lógica de atribuição de endereço ID para o addr no caso de envio de Unicast de configuração.
#define MAX_SIZE_XML_PARAM 32
#define MAX_SIZE_XML_ID 4
#define MAX_SIZE_XML_VALOR 10


enum estados{Aguardando_cmd, Aguardando_LeStatus_motes, Aguardando_Motes_Fase1, Aguardando_Motes_Fase2, Configurando, Aguardando_confirmacao_conf, medicao};
int estado = Aguardando_cmd;

char recvd[256];
char recvd_uc[256];
int pos=0;	

void FSMCoordinator();
#endif
