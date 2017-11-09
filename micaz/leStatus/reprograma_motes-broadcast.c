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

#include "dev/rs232.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);

/*---------------------------------------------------------------------------*/

recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
 /* printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]); */
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

char recvd[256];
int pos=0;	

void Manager2Coordinator(){
	if(strcmp("hello_coordinator", recvd)==0){
		printf("ok\n");
	        leds_toggle(LEDS_RED);
	}else{
		printf("nok\n");
	}
	memset(recvd, 0, sizeof recvd);
	pos=0;
}

static int 
rx_input_byte(unsigned char c){
	recvd[pos++] = c;	
	if(c=='\0' || c=='\n'){
		recvd[--pos]='\0';  //PARA DEBUG COM CUTECOM
		Manager2Coordinator();		
	}
	    leds_on(LEDS_GREEN);
}


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  //Entrada para leitura da serial.
  rs232_set_input(RS232_PORT_0, rx_input_byte);


/*  broadcast_open(&broadcast, 129, &broadcast_call);

  //recebe respostas via unicast
  unicast_open(&uc, 146, &unicast_callbacks);

  //static short int envios=0;
  //for(envios=0; envios<3; envios++) {
  while(1) {
    leds_toggle(LEDS_RED);

    etimer_set(&et, CLOCK_SECOND * 10 + random_rand() % (CLOCK_SECOND * 4));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    char msg[] = "CSMA_MAX_BACKOFF:5";
    
    packetbuf_copyfrom(msg, sizeof(msg));
    broadcast_send(&broadcast);
  } 

*/

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
