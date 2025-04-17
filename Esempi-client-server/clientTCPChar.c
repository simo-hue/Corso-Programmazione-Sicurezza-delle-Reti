#include "network.h"

int main(){
    char receive;
    FILE *fd;
    
    fd = createTCPConnectionFD("localhost", 35000);
    
    fputc('A', fd);
    fflush(fd);

    receive=fgetc(fd);
    fflush(fd);
    printf("[client] %c\n", receive);

    fclose(fd);
}

