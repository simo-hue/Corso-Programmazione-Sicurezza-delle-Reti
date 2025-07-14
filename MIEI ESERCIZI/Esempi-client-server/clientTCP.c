#include "network.h"

int main(void) {
    connection_t connection;
    int request, response;
 	
    // Istanziazione dell’interfaccia socket e apertura della connessione TCP (three-way handshake)
    printf("[CLIENT] Creo una connessione logica col server\n");
    connection = createTCPConnection("localhost", 35000);
    if (connection < 0) {
        printf("[CLIENT] Errore nella connessione al server: %i\n", connection);
    }
    else
    {
        // Chiedo INPUT
        printf("[CLIENT] Inserisci un numero intero:\n");
        scanf("%d", &request);

        // Faccio l'invio tramite TCP
        printf("[CLIENT] Invio richiesta con numero al server\n");
        TCPSend(connection, &request, sizeof(request));

        // Attendo la RISPOSTA
        TCPReceive(connection, &response, sizeof(response));

        // Ed una volta ricevuta
        printf("[CLIENT] Ho ricevuto la seguente risposta dal server: %d\n", response);

        // Chiudo la connessione ( SEMPRE )
        closeConnection(connection);
    }
}


