#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>    
#include "manager.h"
#include <sys/time.h>
#include <termios.h>
#include <string.h>

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


int posByte;

void escreveSerial(char *msg){
	write(fd, msg, strlen(msg));
	usleep ((strlen(msg) + 25) * 100);
}

int leSerialUntil(char *byte, char parada){
	ssize_t size ;
	char *byte_lido;
	int i=0;
	do{
		size = read(fd, byte_lido, 1);
		if(size>0)byte[i++] = *byte_lido;
		else break;
	}while(*byte_lido!='\n');

	if(byte[0]=='\n'){
		 return -1;
	}

	//Tirando o '\n' do fim
	byte[i-1]='\0';
	return size;
	
}

void leSerial(char *byte){
	ssize_t size = read(fd, byte, MAX_BITS_RECEPCAO);
	byte[size-2]='\0';
}


int main(){
   if(abreComunicacaoSerial() < 0){
	printf("O mote coordenador não está acessível via serial\n");
	printf("Verifique se o mote está conectado e/ou se há permissão de acesso\n");
	return ERRO_CONEXAO_SERIAL;
   }else{
	printf("O mote coordenador está conectado via serial\n");
   }
	
	escreveSerial("leStatus_motes\n");	
	sleep(6);
	char byte[MAX_BITS_RECEPCAO];
	int i=0;
	int ret=0;
	while(i<10){
		ret = leSerialUntil(byte, '\n');
		sleep(2);
		if(ret>0)printf("--------------------> %s\n", byte);
 		memset(byte, 0, sizeof byte);
		i++;
		ret=0;
	}
	close(fd);
}
