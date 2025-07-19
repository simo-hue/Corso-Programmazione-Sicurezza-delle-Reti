#include "network.h"

int main(void) {
    int request;
    int response;
    socketif_t socket;
    char hostAddress[MAXADDRESSLEN];
    int port;
    
    socket = createUDPInterface(20002);
     
    printf("Inserisci un numero intero: ( 0 per terminare )\n");
    
    while (1) {
        printf(">> ");
        scanf("%d", &request);
        UDPSend(socket, &request, sizeof(request), "127.0.0.1", 35000);
        if (request == 0) break;
    }
    
    if(UDPReceive(socket, &response, sizeof(response), hostAddress, &port) > 0){
        printf("[CLIENT] Somma ricevuta dal server: %d\n", response);
    } else {
        printf("[CLIENT] Errore nella ricezione della somma\n");
    }
}

