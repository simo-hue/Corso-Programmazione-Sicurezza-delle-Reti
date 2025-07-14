#include "network.h"

int main(void) {
    socketif_t socket;
    char request[]="Ciao sono il client!\n";
    char response[MTU];
    char hostAddress[MAXADDRESSLEN];
    int port;
    
    socket = createUDPInterface(20000);
    
    printf("[CLIENT] Spedisco messaggio al server\n");
    printf("[CLIENT] Contenuto: %s\n", request);
    UDPSend(socket, request, strlen(request), "127.0.0.1", 35000);
     
    UDPReceive(socket, response, MTU, hostAddress, &port);
    printf("[CLIENT] Ho ricevuto un messaggio da host/porta %s/%d\n", hostAddress, port);
    printf("[CLIENT] Contenuto: %s\n", response);
}

