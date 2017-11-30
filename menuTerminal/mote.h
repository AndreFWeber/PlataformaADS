#ifndef MOTE
#define MOTE

#define NUM_MAX_MOTES 256 
#define MAX_SIZE_XML_PARAM 32
#define MAX_SIZE_XML_VALOR 10

static struct unicast_conn uc;
static struct broadcast_conn broadcast;

int lambda=3;
int tamanhoPacote=20;
//int tempoMedicao=10;
uint8_t sink=0; //1 se for sink, 0 se for source
uint8_t start=0; //começa parado...logo start=0
uint8_t configuraBool=0;
uint8_t running=0;//running = 1...Uma das threads (sink or source) está on.

int Sink_totalPacotesRecebidos=0;
int Sink_pacoteID[NUM_MAX_MOTES]={0};//Numero de pacotes por mote.


#endif
