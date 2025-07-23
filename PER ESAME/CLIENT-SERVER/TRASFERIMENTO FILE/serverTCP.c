/*
 * - Invio preliminare della dimensione del file per controllo integrità
 * 4. Invia la dimensione al client (-1 se errore)
 * 
 * PORTA: 35000
 * BUFFER SIZE: 4096 bytes
 * 
 */

#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096  // Buffer per trasferimenti più efficienti -> 4KB alla Volta

int main(void) {
    char filename[256];
    char buffer[BUFFER_SIZE];
    FILE *file;
    socketif_t socket;
    connection_t connection;
    struct stat file_stat;
    long file_size;
    size_t bytes_read, total_sent = 0;

    // Predispongo il tutto per la connessione TCP
    socket = createTCPServer(35000);
    if (socket < 0) {
        printf("[SERVER] Errore nella creazione del socket.\n");
        return -1;
    }

    // Accetto la connessione TCP
    printf("[SERVER] In attesa di connessione sulla porta 35000...\n");
    connection = acceptConnection(socket);
    
    printf("[SERVER] Connessione stabilita\n");

    // Ricevi il nome del file richiesto
    memset(filename, 0, sizeof(filename));
    if (TCPReceive(connection, filename, sizeof(filename) - 1) <= 0) {
        printf("[SERVER] Errore nella ricezione del nome file.\n");
        closeConnection(connection);
        return -1;
    }

    printf("[SERVER] Richiesto il file: %s\n", filename);

    // Verifica se il file esiste e ottieni le sue dimensioni tramite STAT
    if (stat(filename, &file_stat) != 0) {
        printf("[SERVER] File non trovato: %s\n", filename);
        
        // Invia dimensione -1 per indicare errore
        file_size = -1;
        TCPSend(connection, &file_size, sizeof(file_size));
        closeConnection(connection);
        return -1;
    }

    file_size = file_stat.st_size;
    printf("[SERVER] Dimensione file: %ld bytes\n", file_size);

    // Invia la dimensione del file al client
    if (TCPSend(connection, &file_size, sizeof(file_size)) <= 0) {
        printf("[SERVER] Errore nell'invio della dimensione del file.\n");
        closeConnection(connection);
        return -1;
    }

    // Apri il file in modalità binaria
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("[SERVER] Errore nell'apertura del file: %s\n", filename);
        closeConnection(connection);
        return -1;
    }

    // Invia il file a blocchi per maggiore efficienza
    printf("[SERVER] Iniziando trasferimento...\n");
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (TCPSend(connection, buffer, bytes_read) <= 0) {
            printf("[SERVER] Errore durante l'invio dei dati.\n");
            break;
        }
        total_sent += bytes_read;
        
        // Mostra progresso ogni 1MB trasferito
        if (total_sent % (1024 * 1024) == 0) {
            printf("[SERVER] Trasferiti %zu MB\n", total_sent / (1024 * 1024));
        }
    }

    fclose(file);
    
    if (total_sent == file_size) {
        printf("[SERVER] File trasferito completamente (%zu bytes). Chiudo la connessione.\n", total_sent);
    } else {
        printf("[SERVER] Trasferimento incompleto. Trasferiti %zu di %ld bytes.\n", total_sent, file_size);
    }
    
    closeConnection(connection);
    return 0;
}