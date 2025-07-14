#include "network.h"

int main(){
    char receive[MTU];
    socketif_t server;
    FILE *fd;
    
    server = createTCPServer(35000);
    if (server < 0){
        printf("Error: %i\n", server);
        return -1;
    }
    
    while(1){
        fd = acceptConnectionFD(server);
        
        fgets(receive, sizeof(receive), fd);
        fflush(fd);
        printf("%s\n", receive);
        
        fputs("Hi from the server\n", fd);
        fflush(fd);
        
        fclose(fd);
    }
    
    return 0;
}

