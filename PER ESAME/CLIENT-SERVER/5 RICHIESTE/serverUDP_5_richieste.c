/*
    SERVER UDP CON MAX. 5 RICHIESTE
    Questo server UDP risponde a un massimo di 5 richieste ricevute da un client.
    Dopo aver risposto a 5 richieste, il server termina automaticamente.
*/

#include "network.h"

int main(void) {
    int richieste = 0;

    int request;
    int response;
    socketif_t socket;
    char hostAddress[MAXADDRESSLEN];
    int port;
    
    socket = createUDPInterface(35000);
    
    while(richieste < 5) {
    
        printf("[SERVER] Sono in attesa di richieste da qualche client (%d disponibili ancora)\n", 5 - richieste);
    
        UDPReceive(socket, &request, sizeof(request), hostAddress, &port);
        
        printf("[SERVER] Ho ricevuto un messaggio da host/porta %s/%d\n", hostAddress, port);
        
        printf("[SERVER] Contenuto: %d\n", request);
        
        response = request;
    
        UDPSend(socket, &response, sizeof(response), hostAddress, port);

        richieste++;
    }
    printf("[SERVER] Ho sodisfatto 5 richieste.\n");
}

