#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    connection_t conn;
    char filename[100];
    char byte;
    FILE *file;

    // Connessione al server (stesso PC o remoto)
    conn = createTCPConnection("127.0.0.1", 35000);
    if (conn < 0) {
        printf("[CLIENT] Errore nella connessione al server.\n");
        return -1;
    }

    // Chiedi nome file da richiedere
    printf("Nome del file da scaricare: ");
    scanf("%s", filename);

    // Invia nome file al server
    TCPSend(conn, filename, strlen(filename) + 1); // include il terminatore '\0'

    // Apri file per scrittura binaria
    file = fopen(filename, "wb");
    if (file == NULL) {
        printf("[CLIENT] Errore creazione file: %s\n", filename);
        closeConnection(conn);
        return -1;
    }

    // Ricevi un byte alla volta
    while (TCPReceive(conn, &byte, 1) == 1) {
        if ((unsigned char)byte == 255) break; // opzionale per codice fine
        fwrite(&byte, 1, 1, file);
    }

    fclose(file);
    printf("[CLIENT] File salvato come %s\n", filename);
    closeConnection(conn);
    return 0;
}
