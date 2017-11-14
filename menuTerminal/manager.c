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

void escreveSerial(char *msg){
	write(fd, msg, strlen(msg));
	usleep ((strlen(msg) + 25) * 100);
}

int leSerialUntil(char *byte, char parada){
	ssize_t size ;
	char byte_lido;
	int i=0;
					memset(byte, 0, sizeof byte);
	do{
		size = read(fd, &byte_lido, 1);
		
		if(size>0)byte[i++] = byte_lido;
		else{
			if(size==-1)		perror("read() error");
	 		break;
		}
	}while(byte_lido!='\n');

	if(byte[0]=='\n'){
		 return -1;
	}

	//Tirando o '\n' do fim
	byte[i-1]='\0';
	return size;//i-2;
	
}

void leSerial(char *byte){
	ssize_t size = read(fd, byte, MAX_BITS_RECEPCAO);
	byte[size-2]='\0';
}

int esperaSelect(int segundos){
	//Utiliza select para atender uma recepçao via serial ou ao timeout 
	fd_set set;
	struct timeval timeout;

	/* Initialize the file descriptor set. */
	FD_ZERO (&set);
	FD_SET (fd, &set);

	//Set_timeout
	/* Initialize the timeout data structure. */
	timeout.tv_sec = segundos;
	timeout.tv_usec = 0;

	/* select returns 0 if timeout, 1 if input available, -1 if error. */
	return select (FD_SETSIZE, &set, NULL, NULL, &timeout);
}

int fsm_manager(int var){

	int ret_select=0;
	char byte[MAX_BITS_RECEPCAO];

	switch(estado){
		case Esperando_configuracao:
			printf("Esperando_configuracao\n");	
				  
			//!hello_coordinator	
			escreveSerial("hello_coordinator\n");		

			//Ativa o select para esperar uma resposta do coordenador OU o estouro do timer
			ret_select = esperaSelect(1);
			if(ret_select<1){
				//Não recebeu resposta do coordenador
				return ret_select;
			}else{
				//Recebeu a resposta do coordenador
				leSerial(byte);
				if(strcmp("ok", byte)==0){ 
					estado = aguardando_status;
					return COORDENADOR_OK;
				}else{
					return COORDENADOR_NOK;
				}
			}

		case aguardando_status:
			printf("aguardando_status\n");	

			//!LêStatus_Motes	
			escreveSerial("leStatus_motes\n");		

			//Ativa o select para esperar uma resposta do coordenador OU o estouro do timer
			ret_select = esperaSelect(20);
			if(ret_select<1){
				//Não recebeu resposta do coordenador
				printf("NOOP\n");
				return ret_select;
			}else{
				sleep(4);

				//Recebeu a resposta do coordenador
				char cmd[256]={0};
				int ret=0;
				while(1){
					memset(cmd, 0, sizeof cmd);
					if((ret=leSerialUntil(byte, '\n'))>0){
						strncpy(cmd, byte, 11);
						if(strcmp("status_mote", cmd)==0){ //------------------------MUDAR nok para StatusMote 		
							//Arquivo para armazenar dados do experimento
							FILE *fp = fopen(ARQUIVO_EXPERIMENTO, "a");
							if(fp == NULL)
							    return ERRO_OPEN_FILE;

							fprintf(fp, "%s \n", byte);
							//Recebe informações de status dos motes até receber a mensagem fim ou estourar o timeout
							fclose(fp);
						
							memset(cmd, 0, sizeof cmd);
						}
						else{
							if(strcmp("fim", byte)==0){ 
								estado = mostrando_status;				
								return FIM;
							}		
							//return COORDENADOR_NOK;
						}

					}


				}
			}
				

		break;

		case mostrando_status:
		    printf("mostrando_status\n");	
		break;

	}
	return 100;
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
   int pedeOption = 1;	
   do{
	   if(pedeOption){
		   printf("Aperte 1 para ler status ");
		   scanf("%d", &menu_option);	
		   pedeOption=0;
	   }
	   switch(menu_option)
	   {
		    case 1:
			retorno = fsm_manager(1);

			switch(retorno)
			{				
				case COORDENADOR_OK:
					printf("COORDENADOR_OK! \n");
					break;
				case FIM:
					printf("FIM! \n");
					pedeOption=1;
					close(fd);
					return 1;
					break;

				case TIMEOUT:
					printf("Timeout! \n");
					pedeOption=1;
					//estado = Esperando_configuracao;
					break;
				case ERRO_SELECT:
					printf("ERRO: Select retornou -1! \n");
					//estado = Esperando_configuracao;
					pedeOption=1;
					break;
				case ERRO_OPEN_FILE:
					printf("ERRO: Não foi possível criar/abrir um arquivo plataformaDeExperimenos.txt \n");
					//estado = Esperando_configuracao;
					pedeOption=1;
					break;
			}
		        break;
			
		    default:
		   	printf("Invalido\n");
	   }
   }while(menu_option == 1);	
    return 0;
}
