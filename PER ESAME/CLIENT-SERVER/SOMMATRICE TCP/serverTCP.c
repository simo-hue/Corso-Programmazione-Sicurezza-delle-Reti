#include "network.h"

int main(void) {
    int request, response = 0;
    socketif_t socket;
    connection_t connection;
    
    // Istanazio interfaccia SOCKET
    socket = createTCPServer(35000);
    if (socket < 0){
        printf("[SERVER] Errore di creazione del socket: %i\n", socket);
    }
    else
    {
        // Sono aperto a nuove connessioni -> mi metto in ascolto/attesa
        printf("[SERVER] Sono in attesa di richieste di connessione da qualche client\n");    
        connection = acceptConnection(socket);

        // Fine del 3 way handshake
        printf("[SERVER] Connessione instaurata\n");
        
        while(request != 0){
            TCPReceive(connection, &request, sizeof(request));

            // Elaboro la risposta ricevuta
            printf("[SERVER] Somma parziale per il client: %d\n", request);
            response += request;

            // Mando la risposta
            printf("[SERVER] Invio la risposta al client\n");
            TCPSend(connection, &response, sizeof(response));
        }

        printf("Somma totale inviata, posso chiudere la connessione\n");
        // Chiudo la connessione ( SEMPRE )
        closeConnection(connection);
    }
}

