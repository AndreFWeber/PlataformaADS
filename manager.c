#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>    
#include "manager.h"
//#include <sys/select.h>
#include <sys/time.h>

#define ERRO_CONEXAO_SERIAL -1 

int abreComunicacaoSerial(){
    char byte;
    fd = open("/dev/ttyUSB1", O_RDWR);
    return fd;   
}

void escreveSerial(char *msg){
    printf("size = %l", sizeof(msg));
    write(fd, msg, sizeof(msg));
}

void leSerial(){
    ssize_t size = read(fd, &byte, 1);
    printf("Read byte %c\n", byte);
}
 
int fsm_manager(int var){
		    printf("FSM\n");	
	switch(estado){
		case Esperando_configuracao:
			printf("Esperando_configuracao\n");	
				  
			//!hello_coordinator	
			escreveSerial("Hello_coordinator");		

			//Set_timeout
			//Utiliza select para atender uma recepçao via serial ou ao timeout 

			fd_set set;
			struct timeval timeout;

			/* Initialize the file descriptor set. */
			FD_ZERO (&set);
			FD_SET (fd, &set);

			/* Initialize the timeout data structure. */
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;


  			/* select returns 0 if timeout, 1 if input available, -1 if error. */
 			return select (FD_SETSIZE, &set, NULL, NULL, &timeout);

		break;

		case aguardando_status:
		    printf("aguardando_status\n");	
		break;

		case mostrando_status:
		    printf("mostrando_status\n");	
		break;

		default:
		    printf("Estado Inválido\n");	
	}
	return 0;
}

int main() {

   if(abreComunicacaoSerial() < 0){
	printf("O mote coordenador não está acessível via serial\n");
	printf("Verifique se o mote está conectado e/ou se há permissão de acesso\n");
	return ERRO_CONEXAO_SERIAL;
   }else{
	printf("O mote coordenador está conectado via serial\n");
   }

   int menu_option=0;
			int retorno = 0;
   do{
	   printf("Aperte 1 para ler status ");
	   scanf("%d", &menu_option);	


	   switch(menu_option)
	   {
		    case 1:
			retorno = fsm_manager(1);

			if(retorno==1) leSerial();

		        break;

		    default:
		   	printf("Invalido\n");
	   }
   }while(menu_option == 1);	

    return 0;
}
