/*
 * ========================================================================
 * CLIENT TCP PER DOWNLOAD FILE UNIVERSALE
 * ========================================================================
 * 
 * Questo client TCP è progettato per scaricare qualsiasi tipo di file
 * da un server compatibile, garantendo l'integrità completa dei dati
 * e fornendo feedback dettagliato all'utente.
 * 
 * CARATTERISTICHE PRINCIPALI:
 * - Ricezione a blocchi (4KB) per massima efficienza
 * - Controllo dimensione file per verificare integrità
 * - Indicatori di progresso percentuale durante il download
 * - Gestione errori con messaggi informativi
 * - Salvataggio sicuro con prefisso anti-sovrascrittura
 * - Compatibilità universale con tutti i tipi di file
 * 
 * PROTOCOLLO DI COMUNICAZIONE:
 * 1. Si connette al server TCP sulla porta specificata
 * 2. Invia il nome del file da scaricare
 * 3. Riceve la dimensione del file (controllo esistenza)
 * 4. Scarica il file a blocchi verificando il progresso
 * 5. Salva il file localmente con prefisso "downloaded_"
 * 6. Chiude la connessione al completamento
 * 
 * SERVER: 127.0.0.1:35000 (modificabile nel codice)
 * BUFFER SIZE: 4096 bytes
 * 
 * NOTA: I file vengono salvati con prefisso "downloaded_" per evitare
 *       sovrascritture accidentali di file esistenti.
 * ========================================================================
 */

#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096  // Stesso buffer del server

int main(void) {
    connection_t conn;
    char filename[256];
    char buffer[BUFFER_SIZE];
    FILE *file;
    long file_size;
    size_t bytes_received = 0, total_received = 0;
    int bytes_to_read;

    // Connessione al server
    printf("[CLIENT] Tentativo di connessione al server...\n");
    conn = createTCPConnection("127.0.0.1", 35000);
    if (conn < 0) {
        printf("[CLIENT] Errore nella connessione al server.\n");
        return -1;
    }

    printf("[CLIENT] Connesso al server.\n");

    // Chiedi nome file da richiedere
    printf("Nome del file da scaricare: ");
    if (fgets(filename, sizeof(filename), stdin) != NULL) {
        // Rimuovi il newline se presente
        filename[strcspn(filename, "\n")] = '\0';
    }

    // Invia nome file al server (con terminatore null)
    if (TCPSend(conn, filename, strlen(filename) + 1) <= 0) {
        printf("[CLIENT] Errore nell'invio del nome file.\n");
        closeConnection(conn);
        return -1;
    }

    // Ricevi la dimensione del file
    if (TCPReceive(conn, &file_size, sizeof(file_size)) <= 0) {
        printf("[CLIENT] Errore nella ricezione della dimensione del file.\n");
        closeConnection(conn);
        return -1;
    }

    // Controlla se il server ha trovato il file
    if (file_size < 0) {
        printf("[CLIENT] Il file '%s' non è stato trovato sul server.\n", filename);
        closeConnection(conn);
        return -1;
    }

    printf("[CLIENT] Il file ha dimensione: %ld bytes\n", file_size);

    // Crea un nome file locale (aggiunge prefisso per evitare sovrascritture)
    char local_filename[300];
    snprintf(local_filename, sizeof(local_filename), "downloaded_%s", filename);

    // Apri file per scrittura binaria
    file = fopen(local_filename, "wb");
    if (file == NULL) {
        printf("[CLIENT] Errore nella creazione del file: %s\n", local_filename);
        closeConnection(conn);
        return -1;
    }

    // Ricevi il file a blocchi
    printf("[CLIENT] Iniziando il download...\n");
    while (total_received < file_size) {
        // Calcola quanti byte leggere in questo ciclo
        bytes_to_read = (file_size - total_received > BUFFER_SIZE) ? 
                        BUFFER_SIZE : (file_size - total_received);

        // Ricevi i dati
        bytes_received = TCPReceive(conn, buffer, bytes_to_read);
        if (bytes_received <= 0) {
            printf("[CLIENT] Errore nella ricezione dei dati o connessione chiusa.\n");
            break;
        }

        // Scrivi i dati ricevuti nel file
        if (fwrite(buffer, 1, bytes_received, file) != bytes_received) {
            printf("[CLIENT] Errore nella scrittura del file.\n");
            break;
        }

        total_received += bytes_received;

        // Mostra progresso ogni 10% del file
        if (file_size > 0 && (total_received * 10 / file_size) > ((total_received - bytes_received) * 10 / file_size)) {
            printf("[CLIENT] Progresso: %d%% (%zu/%ld bytes)\n", 
                   (int)(total_received * 100 / file_size), total_received, file_size);
        }
    }

    fclose(file);

    if (total_received == file_size) {
        printf("[CLIENT] Download completato! File salvato come '%s'\n", local_filename);
    } else {
        printf("[CLIENT] Download incompleto. Ricevuti %zu di %ld bytes.\n", 
               total_received, file_size);
    }

    closeConnection(conn);
    return 0;
}