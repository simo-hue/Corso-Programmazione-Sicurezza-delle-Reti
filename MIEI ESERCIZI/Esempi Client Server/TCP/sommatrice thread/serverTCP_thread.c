#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

void *gestisci_client(void *arg) {
    connection_t conn = (connection_t)(intptr_t)arg;
    int request, somma = 0;

    printf("[SERVER] Nuovo client connesso.\n");

    while (1) {
        if (TCPReceive(conn, &request, sizeof(request)) <= 0) break;
        if (request == 0) break;
        somma += request;
        printf("[SERVER] Ricevuto: %d → somma = %d\n", request, somma);
    }

    TCPSend(conn, &somma, sizeof(somma));
    closeConnection(conn);
    printf("[SERVER] Connessione chiusa, somma inviata: %d\n", somma);
    return NULL;
}

int main(void) {
    socketif_t socket = createTCPServer(35000);
    printf("[SERVER] In ascolto sulla porta 35000...\n");

    while (1) {
        connection_t conn = acceptConnection(socket);
        pthread_t t;
        pthread_create(&t, NULL, gestisci_client, (void *)(intptr_t)conn);
        pthread_detach(t);
    }
}
