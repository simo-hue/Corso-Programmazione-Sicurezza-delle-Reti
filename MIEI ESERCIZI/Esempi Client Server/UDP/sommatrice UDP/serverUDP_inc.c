// Per eseguirlo: gcc network.c serverUDP.c -o serverUDP -lpthread

#include "network.h"

int main(void) {
    int request;
    socketif_t socket;
    char hostAddress[MAXADDRESSLEN];
    int port;
    int res = 0;

    // Inizializzazione dell’interfaccia socket sulla porta 20000
    socket = createUDPInterface(35000);
    
    printf("[SERVER] Sono in attesa di richieste da qualche client\n");
    
    while(request != 0){
        // Mettiti in attesa dell’arrivo di un messaggio UDP e metti il valore ricevuto in request
        UDPReceive(socket, &request, sizeof(request), hostAddress, &port);
        
        printf("[SERVER] Ho ricevuto un messaggio da host/porta %s/%d con il valore: %d\n", hostAddress, port, request);
        
        // Elaboro la risposta
        res += request;
        printf("[SERVER] Somma Parziale: %d\n", res);
        
        // Invia res a tale mittente tramite UDP
        UDPSend(socket, &res, sizeof(res), hostAddress, port);
    }
}

