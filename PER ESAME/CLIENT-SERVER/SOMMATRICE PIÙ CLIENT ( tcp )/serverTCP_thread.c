#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

// Contatore per capire quanti CLIENT sto gestendo ( puro debug )
int counterGlobal = 0;

// Funzione che gestisce il client -> Invocata per ogni richiesta che ricevo
void *gestisci_client(void *arg) {
    // aggiorno variabile globale
    int counter = counterGlobal + 1;
    counterGlobal++;

    // Tengo traccia della connessione per poi ritornare la SOMMA
    connection_t conn = (connection_t)(intptr_t)arg;

    int request, somma = 0;

    printf("[SERVER ( %d ) ] Nuovo client connesso.\n", counter);

    // Fino a che ricevo valori != 0 allora incremento la somma finale
    while (1) {
        if (TCPReceive(conn, &request, sizeof(request)) <= 0) break;
        if (request == 0) break;
        somma += request;
        printf("[SERVER ( %d ) ] Ricevuto: %d → somma = %d\n", counter, request, somma);
    }

    // Invio il risultato al client e poi chiudo la connessione
    TCPSend(conn, &somma, sizeof(somma));
    closeConnection(conn);
    printf("[SERVER ( %d ) ] Connessione chiusa, somma inviata: %d\n", counter, somma);

    return NULL;
}

int main(void) {
    // Creo il socket e lo inizio a ascoltare sulla porta 35000
    socketif_t socket = createTCPServer(35000);
    printf("[SERVER] In ascolto sulla porta 35000...\n");

    // Per SEMPRE ascolto sulla porta 35000 e creo THREAD per ogni RICHIESTA
    while (1) {
        connection_t conn = acceptConnection(socket);
        pthread_t t;
        pthread_create(&t, NULL, gestisci_client, (void *)(intptr_t)conn);
        pthread_detach(t);
    }
}
