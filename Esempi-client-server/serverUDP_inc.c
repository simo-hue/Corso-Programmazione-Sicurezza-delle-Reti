// Per eseguirlo: gcc network.c serverUDP.c -o serverUDP -lpthread

#include "network.h"

int main(void) {
    int request;
    int response;
    socketif_t socket;
    char hostAddress[MAXADDRESSLEN];
    int port;
    
    // Inizializzazione dell’interfaccia socket sulla porta 20000
    socket = createUDPInterface(35000);
    
    printf("[SERVER] Sono in attesa di richieste da qualche client\n");
    
    UDPReceive(socket, &request, sizeof(request), hostAddress, &port);
    
    printf("[SERVER] Ho ricevuto un messaggio da host/porta %s/%d\n", hostAddress, port);
    
    printf("[SERVER] Contenuto: %d\n", request);
    
    response = request + 1;
    
    UDPSend(socket, &response, sizeof(response), hostAddress, port);
}

