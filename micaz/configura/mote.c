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
#include "lib/random.h" 
#include <math.h>
/*---------------------------------------------------------------------------*/
PROCESS(experiment_process, "Experimento");
PROCESS(sink_process, "sink");
PROCESS(source_process, "source");
AUTOSTART_PROCESSES(&experiment_process);

static process_event_t PROCESS_EVENT_SINK;

/*---------------------------------------------------------------------------*/

int 
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
		return 0;
	}
	if(strcmp("tamanho_pacote", parametro)==0){		
		tamanhoPacote=atoi(valor);
		return 0;
	}
	if(strcmp("modo", parametro)==0){
		if(strcmp("sink", valor)==0){	
			sink=1;
		}else{
			sink=0;
		}
		return 0;
	}
	if(strcmp("destino", parametro)==0){		
		destino=atoi(valor);
		return 0;
	}
	if(strcmp("distribuicao", parametro)==0){		
		distribuicao=atoi(valor);
		return 0;
	}
	if(strcmp("CSMA_MAX_BE", parametro)==0){		
		set_mac_csma(CSMA_MAX_BE_, atoi(valor));
		return 0;
	}
	if(strcmp("CSMA_MIN_BE", parametro)==0){		
		set_mac_csma(CSMA_MIN_BE_, atoi(valor));
		return 0;
	}
	if(strcmp("macMaxCSMABackoffs", parametro)==0){		
		set_mac_csma(CSMA_MAX_BACKOFF_, atoi(valor));
		return 0;
	}
	if(strcmp("macMaxFramesRetries", parametro)==0){	 	
		set_mac_csma(CSMA_MAX_MAX_FRAME_RETRIES_, atoi(valor));
		return 0;
	}

	return -1;

}

void send_msg(linkaddr_t addr, char* msg){
    packetbuf_copyfrom(msg, strlen(msg)+1);
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&uc, &addr);
    }
}
/*---------------------------------------------------------------------------*/


static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("chegou %s do %d por broadcast\n", (char *)packetbuf_dataptr(), from->u8[0]);
  
 if(from->u8[0]!=1){ //O coordenador da rede tem ID 1...
	if(start==1 && running==1){
//		estatisticas_sink(from->u8[0], (char *)packetbuf_dataptr());
		if(sink==1){
			process_post_synch(&sink_process, PROCESS_EVENT_SINK, &from->u8[0]);
		}	
	}
  }else{//...Se recebeu do ID 1, então veio do coordenador
	  if(strcmp((char *)packetbuf_dataptr(), "start")==0){
		send_msg(*from, "ok");
		start=1;
	  }
	  if(strcmp((char *)packetbuf_dataptr(), "stop")==0){
		//send_msg(*from, "ok");
		start=0;
	  }
	  if(strcmp((char *)packetbuf_dataptr(), "broadcast_status")==0)
		send_msg(*from, "ok");

	  if(configuraBool==1){
		int cret = configura((char *)packetbuf_dataptr());
		if(cret==0)
			send_msg(*from, "ok");
		else
			send_msg(*from, "nok");
		configuraBool=0;
	  }
	  if(strcmp((char *)packetbuf_dataptr(), "configura")==0){
		configuraBool=1;	
	  }
  }
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  if(from->u8[0]!=1){ //O coordenador da rede tem ID 1...
	if(start==1 && running==1){
//		estatisticas_sink(from->u8[0], (char *)packetbuf_dataptr());
		if(sink==1){
			process_post_synch(&sink_process, PROCESS_EVENT_SINK, &from->u8[0]);
		}	
	}
  }else{
	  if(configuraBool==1){
		int cret = configura((char *)packetbuf_dataptr());
		if(cret==0)
			send_msg(*from, "ok");
		else
			send_msg(*from, "nok");
		configuraBool=0;
	  }
	  if(strcmp((char *)packetbuf_dataptr(), "configura")==0){
		configuraBool=1;	
	  }

  }
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};

/*--------------------------SINK------------------------------------*/

PROCESS_THREAD(sink_process, ev, data)
{

  PROCESS_BEGIN();
	  while(1) {
	    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_SINK);
	    Sink_totalPacotesRecebidos++;
	    Sink_pacoteID[*(int*)data]++;
	  }
  PROCESS_END();
}

/*--------------------------SOURCE------------------------------------*/
static unsigned int nextPacketTime(int lambda){
	return (-lambda) * logf( random_rand() / (float)RANDOM_RAND_MAX );
}
PROCESS_THREAD(source_process, ev, data)
{
  PROCESS_BEGIN();
	  while(1) {
		    static struct etimer et;
		    linkaddr_t addr;

		    //Verifica se usa distribuição exponencial ou cte. bitrate
	
		    unsigned int prox_envio=lambda;//se for cte. então usa o lambda direto
                    if(distribuicao==1)//Vai ser distribuiçao exponencial
			prox_envio=nextPacketTime(lambda);

		    etimer_set(&et, prox_envio*CLOCK_SECOND);
		    
		    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		    packetbuf_copyfrom("Hello", tamanhoPacote);
		    addr.u8[0] = destino;
		    addr.u8[1] = 0;
		    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
		      unicast_send(&uc, &addr);
			Source_totalPacotesEnviados++;
		    }
	    }
  PROCESS_END();
}

/*--------------------------MAIN------------------------------------*/



char cfg[20];
PROCESS_THREAD(experiment_process, ev, data)
{
//  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();
  //Evento especifico para o sink desta plataforma
  PROCESS_EVENT_SINK = process_alloc_event();

  //NECESSÁRIO PARA PROGRAMAR O MICAZ
  node_id_burn(100);
  unicast_open(&uc, 146, &unicast_callbacks);
  broadcast_open(&broadcast, 129, &broadcast_call);

  linkaddr_t coordenador;
  coordenador.u8[0] = 1;
  coordenador.u8[1] = 0;

  static struct etimer et;

  while(1) {
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    if(start==1 && running==0){//Se recebeu um start
	running=1;

	if(sink==1){//Este mote será sink!
		process_start(&sink_process, NULL);
     		leds_toggle(LEDS_RED);
        }else{
		process_start(&source_process, NULL);		
	        leds_toggle(LEDS_GREEN);
	}
    }
    if(start==0 && running==1){
	//Quando receber o stop
	//pede para a thread repassar as estatisticas e então envia o sinal de exit.
	if(sink==1){//Este mote será sink!
		process_exit(&sink_process);
		linkaddr_t addr;
		sprintf(cfg, "TotalPacotes/%d", Sink_totalPacotesRecebidos);
		packetbuf_copyfrom(&cfg, 20);
		addr.u8[0] = 1;
		addr.u8[1] = 0;
		if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
			unicast_send(&uc, &addr);
		}
	}else{
		process_exit(&source_process);		
		linkaddr_t addr;
		sprintf(cfg, "TotalPacotes/%d", Source_totalPacotesEnviados);
		packetbuf_copyfrom(&cfg, 20);
		addr.u8[0] = 1;
		addr.u8[1] = 0;
		if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
			unicast_send(&uc, &addr);
		}
	}
	running=0;
     
        //printf("valores %d e %d", Sink_totalPacotesRecebidos, Sink_pacoteID[15]);
	leds_on(LEDS_ALL);
	etimer_set(&et, 5*CLOCK_SECOND);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	Sink_totalPacotesRecebidos=0;
	Source_totalPacotesEnviados=0;
	leds_off(LEDS_ALL);

    }

  }

  PROCESS_END();

}
