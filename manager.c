#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>    
#include "manager.h"

int fsm_manager(int estado){

	switch(estado){
		case Esperando_configuracao:
			break;
		case aguardando_status:
			break;
		case mostrando_status:
			break;
	}
	return 0;
}

int main() {
 /*   char byte;
    int fd = open("/dev/ttyUSB1", O_RDWR);
    write(fd, "teste", 5);
    //ssize_t size = read(fd, &byte, 1);
    //printf("Read byte %c\n", byte);
*/

   print


    return 0;
}
