#include "network.h"

int main(void) {
    int request, response;
    socketif_t socket;
    connection_t connection;
    
    socket = createTCPServer(35000);
    if (socket < 0){
        printf("[SERVER] Errore di creazione del socket: %i\n", socket);
    }
    else
    {
        printf("[SERVER] Sono in attesa di richieste di connessione da qualche client\n");    
        connection = acceptConnection(socket);
        printf("[SERVER] Connessione instaurata\n");
        TCPReceive(connection, &request, sizeof(request));
        printf("[SERVER] Ho ricevuto la seguente richiesta dal client: %d\n", request);
        response = request + 1;
        printf("[SERVER] Invio la risposta al client\n");
        TCPSend(connection, &response, sizeof(response));
        closeConnection(connection);
    }
}

