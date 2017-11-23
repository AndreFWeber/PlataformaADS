#ifndef COORDENADOR
#define COORDENADOR

enum estados{Aguardando_cmd, Aguardando_LeStatus_motes, Aguardando_Motes_Fase1, Aguardando_Motes_Fase2, Configurando};
int estado = Aguardando_cmd;

char recvd[256];
int pos=0;	

void FSMCoordinator();
#endif
