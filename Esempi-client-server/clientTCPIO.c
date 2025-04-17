#include "network.h"

int main(){
    char receive[MTU];
    FILE *fd;
    
    fd = createTCPConnectionFD("localhost", 35000);
    
    fputs("Hi from the client\n", fd);
    fflush(fd);
    
    fgets(receive, sizeof(receive), fd);
    fflush(fd);
    printf("%s\n", receive);

    fclose(fd);
}

