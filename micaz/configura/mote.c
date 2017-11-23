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
/*---------------------------------------------------------------------------*/
PROCESS(broadcast_process, "Broadcast");
PROCESS(experiment_process, "Experimento");

AUTOSTART_PROCESSES(&broadcast_process, &experiment_process);

/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  leds_toggle(LEDS_BLUE);//AMARELO!!!!
  printf("broadcast message received from %d.%d: '%s'\n", from->u8[0], from->u8[1], (char *)packetbuf_dataptr());

  printf("responde broadcas\n");
  send_msg(*from);

}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;

/*---------------------------------------------------------------------------*/

recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  printf("unicast message received from %d.%d\n", from->u8[0], from->u8[1]);
  send_msg(*from);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
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
/*---------------------------------------------------------------------------*/

void send_msg(linkaddr_t addr){
    packetbuf_copyfrom("Hello", 5);
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&uc, &addr);
      leds_toggle(LEDS_GREEN);
    }
}

PROCESS_THREAD(experiment_process, ev, data)
{
  PROCESS_BEGIN();

  //NECESSÁRIO PARA PROGRAMAR O MICAZ
  node_id_burn(3);

  unicast_open(&uc, 146, &unicast_callbacks);

  while(1) {
    static struct etimer et;
    linkaddr_t addr;
    
    etimer_set(&et, 5*CLOCK_SECOND);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));


    addr.u8[0] = 1;
    addr.u8[1] = 0;

 //   send_msg(addr);

  }


  PROCESS_END();

}
