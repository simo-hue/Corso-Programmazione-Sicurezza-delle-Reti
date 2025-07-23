#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {

    // Creazione della connessione TCP -> 3 way handshake
    connection_t conn = createTCPConnection("127.0.0.1", 35000);
    int request, somma;

    printf("[CLIENT] Inserisci numeri interi. 0 per terminare:\n");

    // Continuo a chiedere INPUT fino a che NON ricevo 0
    while (1) {
        printf(">> ");
        scanf("%d", &request);
        TCPSend(conn, &request, sizeof(request));
        if (request == 0) break;
    }

    // Ricezione della somma
    if (TCPReceive(conn, &somma, sizeof(somma)) > 0) {
        printf("[CLIENT] Somma ricevuta dal server: %d\n", somma);
    } else {
        printf("[CLIENT] Errore nella ricezione della somma\n");
    }

    // Chiusura della connessione
    closeConnection(conn);
    return 0;
}
