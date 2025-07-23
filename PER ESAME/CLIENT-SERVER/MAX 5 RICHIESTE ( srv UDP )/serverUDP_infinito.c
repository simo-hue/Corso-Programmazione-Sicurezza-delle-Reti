/*
    SERVER SENZA LIMITE DI RICHIESTE
    Questo server UDP risponde a qualsiasi richiesta ricevuta da un client, senza un limite di
    richieste. Continua a ricevere e rispondere finché non viene terminato manualmente.
*/

#include "network.h"

int main(void) {
    int request;
    int response;
    socketif_t socket;
    char hostAddress[MAXADDRESSLEN];
    int port;
    
    socket = createUDPInterface(35000);
    
    // cliclo infinito e quindi NON TERMINERÀ mai
    while(true) {
    
        printf("[SERVER] Sono in attesa di richieste da qualche client\n");
    
        // Rimango in attesa per una richiesta UDP
        UDPReceive(socket, &request, sizeof(request), hostAddress, &port);
        
        printf("[SERVER] Ho ricevuto un messaggio da host/porta %s/%d\n", hostAddress, port);
        
        printf("[SERVER] Contenuto: %d\n", request);
        
        response = request;
    
        // Mando risposta al client
        UDPSend(socket, &response, sizeof(response), hostAddress, port);
    }
    printf("[SERVER] Ho sodisfatto 5 richieste.\n");
}



