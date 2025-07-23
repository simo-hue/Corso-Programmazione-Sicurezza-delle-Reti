#include "network.h"

int main(void) {
    int request;
    int response;
    socketif_t socket;
    char hostAddress[MAXADDRESSLEN];
    int port;
    
    socket = createUDPInterface(20000);
     
    printf("Inserisci un numero intero: ( 0 per terminare )\n");
    
    // Continuo ad inviare al server NUMERI IN INPUT fino a che NON ricevo lo 0
    while (1) {
        printf(">> ");
        scanf("%d", &request);
        UDPSend(socket, &request, sizeof(request), "127.0.0.1", 35000);
        if (request == 0) break;
    }
    
    // Rimango in attesa per una richiesta UDP
    if(UDPReceive(socket, &response, sizeof(response), hostAddress, &port) > 0){
        printf("[CLIENT] Somma ricevuta dal server: %d\n", response);
    } else {
        printf("[CLIENT] Errore nella ricezione della somma\n");
    }
}

