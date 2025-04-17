#include "network.h"

int main(void) {
    connection_t connection;
    int request, response;
 	
    printf("[CLIENT] Creo una connessione logica col server\n");
    connection = createTCPConnection("localhost", 35000);
    if (connection < 0) {
        printf("[CLIENT] Errore nella connessione al server: %i\n", connection);
    }
    else
    {
        printf("[CLIENT] Inserisci un numero intero:\n");
        scanf("%d", &request);
        printf("[CLIENT] Invio richiesta con numero al server\n");
        TCPSend(connection, &request, sizeof(request));
        TCPReceive(connection, &response, sizeof(response));
        printf("[CLIENT] Ho ricevuto la seguente risposta dal server: %d\n", response);
        closeConnection(connection);
    }
}


