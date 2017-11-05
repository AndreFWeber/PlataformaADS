#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>    
int main() {
    char byte;
    int fd = open("/dev/ttyUSB1", O_RDWR);
    write(fd, "teste", 5);
    //ssize_t size = read(fd, &byte, 1);
    //printf("Read byte %c\n", byte);
    return 0;
}
