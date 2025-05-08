#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char filename[100];
    char byte;
    FILE *file;
    socketif_t socket;
    connection_t connection;

    // Predispongo il tutto per la connessione TCP
    socket = createTCPServer(35000);
    if (socket < 0) {
        printf("[SERVER] Errore nella creazione del socket.\n");
        return -1;
    }

    // Sono pronto
    printf("[SERVER] In attesa di connessione...\n");
    connection = acceptConnection(socket);
    
    // 3-way handshake fatto
    printf("[SERVER] Connessione instaurata\n");

    // Ricevi il nome del file richiesto
    if (TCPReceive(connection, filename, sizeof(filename)) <= 0) {
        printf("[SERVER] Errore nella ricezione del nome file.\n");
        closeConnection(connection);
        return -1;
    }

    printf("[SERVER] Richiesto il file: %s\n", filename);

    // Apri il file da inviare
    file = fopen(filename, "rb");  // binario per compatibilità
    if (file == NULL) {
        printf("[SERVER] Errore apertura file: %s\n", filename);
        byte = -1;  // codice errore al client
        TCPSend(connection, &byte, 1);
        closeConnection(connection);
        return -1;
    }

    // Invia byte per byte
    while (fread(&byte, 1, 1, file) == 1) {
        if (TCPSend(connection, &byte, 1) <= 0) break;
    }

    fclose(file);
    printf("[SERVER] File inviato. Chiudo la connessione.\n");
    closeConnection(connection);
    return 0;
}
