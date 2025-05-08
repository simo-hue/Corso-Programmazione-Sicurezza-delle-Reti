#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
    connection_t conn = createTCPConnection("127.0.0.1", 35000);
    int request, somma;

    printf("[CLIENT] Inserisci numeri interi. 0 per terminare:\n");

    while (1) {
        printf(">> ");
        scanf("%d", &request);
        TCPSend(conn, &request, sizeof(request));
        if (request == 0) break;
    }

    if (TCPReceive(conn, &somma, sizeof(somma)) > 0) {
        printf("[CLIENT] Somma ricevuta dal server: %d\n", somma);
    } else {
        printf("[CLIENT] Errore nella ricezione della somma\n");
    }

    closeConnection(conn);
    return 0;
}
