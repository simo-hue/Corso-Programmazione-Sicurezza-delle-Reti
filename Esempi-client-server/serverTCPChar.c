#include "network.h"

int main(){
    char receive;
    socketif_t server;
    FILE *fd;
    
    server = createTCPServer(35000);
    if (server < 0){
        printf("Error: %i\n", server);
        return -1;
    }
    
    while(true){
        fd = acceptConnectionFD(server);
        
        receive=fgetc(fd);
        fflush(fd);
        printf("[server] %c\n", receive);
        
        fputc('B', fd);
        fflush(fd);
    
        fclose(fd);
    }
    
    return 0;
}

