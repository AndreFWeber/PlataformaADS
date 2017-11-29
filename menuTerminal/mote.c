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
#include "net/rime/collect.h"
#include <stdio.h>
#include "node-id.h"
#include <stdlib.h>
#include "mote.h"
#include "net/mac/csma_variable.h"
#include "cc2420.h"
/*---------------------------------------------------------------------------*/
PROCESS(broadcast_process, "Broadcast");
PROCESS(experiment_process, "Experimento");

AUTOSTART_PROCESSES(&broadcast_process, &experiment_process);

static struct unicast_conn uc;
static struct broadcast_conn broadcast;

int lambda=3;
int tamanhoPacote=20;
//int tempoMedicao=10;
uint8_t sink=0; //1 se for sink, 0 se for source

/*---------------------------------------------------------------------------*/

void 
configura(char *cfg){
	//Divisoes da string
	char* parametro = malloc(MAX_SIZE_XML_PARAM);
	char* valor =  malloc(MAX_SIZE_XML_VALOR);

	char* token;
	char* string;
	string = strdup(cfg);
	uint8_t split=0;

	while ((token = strsep(&string, "/")) != NULL)
	{
		if(split==0){
		 	memcpy(parametro, token, strlen(token)+1);
		}else{
		   	memcpy(valor, token, strlen(token)+1);
		}
		split++;
	}

	if(strcmp("lambda", parametro)==0){	
		lambda=atoi(valor);
	}
	if(strcmp("tamanho_pacote", parametro)==0){		
		tamanhoPacote=atoi(valor);
	}
	if(strcmp("modo", parametro)==0){	
		if(strcmp("sink", valor)==0){	
			sink=1;
		}else{
			sink=0;
		}
	}
	if(strcmp("CSMA_MAX_BE", parametro)==0){		
		set_mac_csma(CSMA_MAX_BE_, atoi(valor));
	}
	if(strcmp("CSMA_MIN_BE", parametro)==0){		
		set_mac_csma(CSMA_MIN_BE_, atoi(valor));
	}
	if(strcmp("macMaxCSMABackoffs", parametro)==0){		
		set_mac_csma(CSMA_MAX_BACKOFF_, atoi(valor));
	}
	if(strcmp("macMaxFramesRetries", parametro)==0){	 	
		set_mac_csma(CSMA_MAX_MAX_FRAME_RETRIES_, atoi(valor));
	}
}

void send_msg(linkaddr_t addr, char* msg){
    packetbuf_copyfrom(msg, strlen(msg)+1);
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&uc, &addr);
      leds_toggle(LEDS_GREEN);
    }
}
/*---------------------------------------------------------------------------*/

uint8_t configuraBool=0;
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  if(strcmp((char *)packetbuf_dataptr(), "start")==0)
	send_msg(*from, "ok");

  if(strcmp((char *)packetbuf_dataptr(), "stop")==0)
	send_msg(*from, "ok");

  if(strcmp((char *)packetbuf_dataptr(), "broadcast_status")==0)
	send_msg(*from, "ok");

  if(configuraBool==1){
	configura((char *)packetbuf_dataptr());
	send_msg(*from, "ok");
	configuraBool=0;
  }
  if(strcmp((char *)packetbuf_dataptr(), "configura")==0){
	configuraBool=1;	
  }
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  if(configuraBool==1){
	configura((char *)packetbuf_dataptr());
	send_msg(*from, "ok");
	configuraBool=0;
  }
  if(strcmp((char *)packetbuf_dataptr(), "configura")==0){
	configuraBool=1;	
  }
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_process, ev, data)
{ 
  static struct etimer et;
  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  while(1){
    etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 4));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  } 

  PROCESS_END();
}

PROCESS_THREAD(experiment_process, ev, data)
{
  PROCESS_BEGIN();

  //NECESSÁRIO PARA PROGRAMAR O MICAZ
  node_id_burn(200);

  unicast_open(&uc, 146, &unicast_callbacks);

  while(1) {
    static struct etimer et;
  //  linkaddr_t addr;
    
    etimer_set(&et, 5*CLOCK_SECOND);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));


    //addr.u8[0] = 1;
    //addr.u8[1] = 0;

 //   send_msg(addr);

  }


  PROCESS_END();

}
