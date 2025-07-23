#include "network.h"

int main(void) {
    int request;
    socketif_t socket;
    char hostAddress[MAXADDRESSLEN];
    int port;
    int somma = 0;
    socket = createUDPInterface(35000);
    
    printf("[SERVER] Sono in attesa di richieste da qualche client\n");
    
    // Fino a che NON ricevo il "CODICE" di USCITA -> 0
    while(request != 0){

            // Ricevo la richiesta dal client
            UDPReceive(socket, &request, sizeof(request), hostAddress, &port);
            printf("[SERVER] Ho ricevuto un messaggio da host/porta %s/%d, con contenuto %d\n", hostAddress, port, request);
        
            // Elaboro la risposta ricevuta
            somma += request;
            printf("[SERVER] Somma parziale: %d\n", somma);

        }
        
        // Mando la risposta finale -> SOMMA EFFETTIVA
        printf("[SERVER] Invio la risposta al client con SOMMA COMPLETA: %d\n", somma);
        UDPSend(socket, &somma, sizeof(somma), hostAddress, port);
    
}

