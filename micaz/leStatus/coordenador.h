#ifndef COORDENADOR
#define COORDENADOR

enum estados{Aguardando_LeStatus_motes, Aguardando_Motes_Fase1, Aguardando_Motes_Fase2};
int estado = Aguardando_LeStatus_motes;

char recvd[256];
int pos=0;	

void FSMCoordinator();
#endif
