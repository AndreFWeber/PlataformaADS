/**
 * Coordenador da rede (reprograma_motes-broadcast) envia uma nova configuração aos motes (mote)
 * - Acende LED vermelho do MICAz 
 *
 * Motes acendem o LED amarelo ao receberem uma mensagem de broadcast e verde ao receber uma mensagem de 
 * outro mote.
 */

#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/leds.h"
#include <stdio.h>
#include "coordenador.h"
#include "dev/rs232.h"
#include "node-id.h"
#include <stdlib.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define NUM_MAX_MOTES 9

/*---------------------------------------------------------------------------*/
PROCESS(coordenador_process, "Coordenador");
AUTOSTART_PROCESSES(&coordenador_process);

/*---------------------------------------------------------------------------*/
  int dados_unicast=0;
int motes[NUM_MAX_MOTES]={0};
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  //printf("unicast message received from %d.%d\n", from->u8[0], from->u8[1]); 
  int i;
  for(i=0; i<NUM_MAX_MOTES; i++){
	if(motes[i]==from->u8[0]) return;
  }
  
  //Assumo que só me responde quem ainda não está na lista!	
  motes[dados_unicast++]=from->u8[0];
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  leds_toggle(LEDS_BLUE);
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;

/*---------------------------------------------------------------------------*/

static int 
rx_input_byte(unsigned char c){
	recvd[pos++] = c;	
	if(c=='\0' || c=='\n'){
		recvd[--pos]='\0'; 
		FSMCoordinator();		
	}
	leds_toggle(LEDS_GREEN);
	return 1; 
}

int broadcast_status=0;

void sendBroadcast(char* msg){
    //char msg[] = "broadcast_status";
    packetbuf_copyfrom(msg, sizeof(msg));
    broadcast_send(&broadcast);
    broadcast_status=1;
   // printf("broadcast\n");
}

void FSMCoordinator(){
	switch (estado){
		case Aguardando_cmd:
			if(strcmp("hello_coordinator", recvd)==0){
				//!ok
				printf("ok\n");
				leds_toggle(LEDS_RED);
			}
			else
			if(strcmp("leStatus_motes", recvd)==0){
				estado=Aguardando_LeStatus_motes;
				FSMCoordinator();
			}
			else
			if(strcmp("configura", recvd)==0){
				estado=Configurando;
				printf("ok\n");	
			}
			break;

		case Aguardando_LeStatus_motes:
			//!Broadcast_status 
			sendBroadcast("broadcast_status");
			estado = Aguardando_Motes_Fase1;
			break;
		case Aguardando_Motes_Fase1:
			printf("status_mote %d,%d,%d,%d,%d,%d,%d,%d,%d\n", motes[0], motes[1], motes[2], motes[3], motes[4], motes[5], motes[6], motes[7], motes[8] );

			//!Broadcast_status 
			sendBroadcast("broadcast_status");
			estado = Aguardando_Motes_Fase2;
			break;
		case Aguardando_Motes_Fase2:
			printf("status_mote %d,%d,%d,%d,%d,%d,%d,%d,%d\n", motes[0], motes[1], motes[2], motes[3], motes[4], motes[5], motes[6], motes[7], motes[8] );
			printf("fim\n");
			estado = Aguardando_cmd;
			break;

		case Configurando: ;
			//Divisoes da string
			char* parametro =  malloc(sizeof(char));
			char* ID  =  malloc(sizeof(char));
			char* valor  =  malloc(sizeof(char));

			char* token;
			char* string;
			string = strdup(recvd);
			int split=0;

			while ((token = strsep(&string, "/")) != NULL)
			{
			  if(split==0)
			 	memcpy(parametro, token, sizeof *token);
			    else
			    if(split==1)
			   	memcpy(ID, token, sizeof *token);
			    else
			    if(split==2)
			   	memcpy(valor, token, sizeof *token);
			    split++;
			}
	
			if(strcmp("0", ID)==0){//Envia as configurações por broadcast e espera
				printf("Broadcast");
			}

			free(parametro);
			free(ID);
			free(valor);

			break;
	}

	memset(recvd, 0, sizeof recvd);
	pos=0;	
	memset(motes, 0, sizeof motes);
	dados_unicast=0;
}



/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coordenador_process, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();
  node_id_burn(1);

  //Entrada para leitura da serial.
  rs232_set_input(RS232_PORT_0, rx_input_byte);


  broadcast_open(&broadcast, 129, &broadcast_call);

  //recebe respostas via unicast
  unicast_open(&uc, 146, &unicast_callbacks);

 //Se enviou um broadcast!	
  while(1) {
	  etimer_set(&et, CLOCK_SECOND/50); //----------------------REVER ISSO! PQ SÓ FUNCIONA SE TEM O TIMER E O PROCESS_WAIT...
	  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	  if(broadcast_status==1){
		  broadcast_status=0;
		 // printf("Esperando timeout!\n");
		 // if(dados_unicast==0) {
			  etimer_set(&et, CLOCK_SECOND * 2);
			  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			  //Esperou por 2 segundos uma resposta...Quem não respondeu vai ter que responder de novo na fase 2
			 // printf("Timeout de espera do unicast!\n");
			  FSMCoordinator();
		  //}
	  }

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
