#include "network.h"

int main(void) {
    int request;
    int response;
    socketif_t socket;
    char hostAddress[MAXADDRESSLEN];
    int port;
    
    socket = createUDPInterface(20000);
     
    printf("Inserisci un numero intero:\n");
    
    scanf("%d", &request);
    
    UDPSend(socket, &request, sizeof(request), "127.0.0.1", 35000);
     
    UDPReceive(socket, &response, sizeof(response), hostAddress, &port);
     
    printf("[CLIENT] Ho ricevuto un messaggio da host/porta %s/%d\n", hostAddress, port);
    
    printf("[CLIENT] Contenuto: %d\n", response);
}

