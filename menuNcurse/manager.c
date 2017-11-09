#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>    
#include "manager.h"
#include <sys/time.h>
#include <termios.h>

static struct termios oldtio, newtio;
int abreComunicacaoSerial(){
    char byte;
    fd = open("/dev/ttyUSB1", O_RDWR);

    tcgetattr (fd, &oldtio);    /* save current serial port settings */
    bzero (&newtio, sizeof (newtio));   /* clear struct for new port settings */

    /* 
     *BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
     *CRTSCTS : output hardware flow control (only used if the cable has
     *all necessary lines. )
     *CS8     : 8n1 (8bit,no parity,1 stopbit)
     *CLOCAL  : local connection, no modem contol
     *CREAD   : enable receiving characters
     **/
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;

    /*
     *IGNPAR  : ignore bytes with parity errors
     *ICRNL   : map CR to NL (otherwise a CR input on the other computer
     *          will not terminate input)
     *          otherwise make device raw (no other input processing)
     **/
    newtio.c_iflag = IGNPAR | ICRNL;

    /* 
     * now clean the modem line and activate the settings for the port
     **/
    tcflush (fd, TCIFLUSH);
    tcsetattr (fd, TCSANOW, &newtio);

    /*
     * terminal settings done, return file descriptor
     **/

    return fd;   
}

void escreveSerial(char *msg){
	write(fd, msg, strlen(msg));
	usleep ((strlen(msg) + 25) * 100);
}

void leSerial(char *byte){
	ssize_t size = read(fd, byte, MAX_BITS_RECEPCAO);
	byte[size-2]='\0';
}

 

int fsm_manager(int var){
		    //printf("FSM\n");	
	switch(estado){
		case Esperando_configuracao:
			//printf("Esperando_configuracao\n");	
				  
			//!hello_coordinator	
			escreveSerial("hello_coordinator\n");		

			//Utiliza select para atender uma recepçao via serial ou ao timeout 
			fd_set set;
			struct timeval timeout;

			/* Initialize the file descriptor set. */
			FD_ZERO (&set);
			FD_SET (fd, &set);

			//Set_timeout
			/* Initialize the timeout data structure. */
			timeout.tv_sec = 50;
			timeout.tv_usec = 0;

  			/* select returns 0 if timeout, 1 if input available, -1 if error. */
 			int ret_select = select (FD_SETSIZE, &set, NULL, NULL, &timeout);
			if(ret_select<1){
				//Não recebeu resposta do coordenador
				return ret_select;
			}else{
				//Recebeu a resposta do coordenador
				char byte[MAX_BITS_RECEPCAO];
				leSerial(byte);
				if(strcmp("ok", byte)==0){ 
					estado = aguardando_status;
					return COORDENADOR_OK;
				}else{
					return COORDENADOR_NOK;
				}
			}

		case aguardando_status:
			//printf("aguardando_status\n");	
		break;

		case mostrando_status:
		   // printf("mostrando_status\n");	
		break;
	}
	return 0;
}

