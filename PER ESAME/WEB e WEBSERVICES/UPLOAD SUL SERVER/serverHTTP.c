// Eseguendo http://127.0.0.1:8000/ o su localhost NON cambia NULLA perchè entrambi puntano all’interfaccia di loopback 
// Se provo ad aprire un ALTRO server NON FUNZIONA, perchè NON posso avere 2 processi sulla stessa PORTA dello Stesso dispositivo

// PER WIRESHARK applico questo filtro: http && tcp.port == 8000
// Eseguendo http://127.0.0.1:8000/ o su localhost NON cambia NULLA perchè entrambi puntano all’interfaccia di loopback 
// Se provo ad aprire un ALTRO server NON FUNZIONA, perchè NON posso avere 2 processi sulla stessa PORTA dello Stesso dispositivo

// PER WIRESHARK applico questo filtro: http && tcp.port == 8000

#include "network.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main() {
    // 1) HTML di risposta dopo l'upload
    char *HTMLResponse =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "\r\n"
        "<html><head><title>Upload ricevuto</title></head>"
        "<body><h1>File caricato con successo.</h1></body></html>\r\n";

    // 2) Variabili principali
    socketif_t sockfd;
    FILE *connfd;
    long content_length = 0;
    char request[MTU];
    char method[10];

    // 3) Creazione del server TCP sulla porta 8000
    sockfd = createTCPServer(8000);
    if (sockfd < 0) {
        printf("[SERVER] Errore creazione server: %i\n", sockfd);
        return -1;
    }

    while (true) {
        // 4) Attesa e accettazione di una nuova connessione
        connfd = acceptConnectionFD(sockfd);
        printf("[DEBUG] Connessione accettata\n");

        // 5) Lettura della prima riga: "POST /action HTTP/1.1"
        if (!fgets(request, sizeof(request), connfd)) {
            printf("[DEBUG] Errore lettura prima riga request\n");
            fclose(connfd);
            continue;
        }
        printf("[DEBUG] Prima riga: %s", request);
        strcpy(method, strtok(request, " ")); // metodo (es. "POST")
        printf("[DEBUG] Metodo estratto: %s\n", method);

        // 6) Lettura delle intestazioni HTTP per ottenere Content-Length e boundary
        char boundary[100] = {0};
        content_length = 0;
        while (fgets(request, sizeof(request), connfd) && strcmp(request, "\r\n") != 0) {
            printf("[DEBUG] Header: %s", request);
            // Cerca Content-Length
            if (strncasecmp(request, "Content-Length:", 15) == 0) {
                content_length = atol(request + 15);
                printf("[DEBUG] Content-Length trovato: %ld\n", content_length);
            }
            // Cerca boundary in multipart/form-data
            if (strstr(request, "boundary=") != NULL) {
                char *b = strstr(request, "boundary=") + 9;
                int len = 0;
                while (b[len] != '\r' && b[len] != '\n' && b[len] != '\0' && len < (int)sizeof(boundary)-1) {
                    boundary[len] = b[len];
                    len++;
                }
                boundary[len] = '\0';
                printf("[DEBUG] Boundary trovato: %s\n", boundary);
            }
        }
        printf("[DEBUG] Fine header, inizio corpo multipart\n");

        // 7) Gestione POST per file upload
        if (strcmp(method, "POST") == 0 && content_length > 0) {
            // a) Allocazione buffer per tutto il corpo
            char *body = malloc(content_length);
            if (!body) {
                printf("[SERVER] Errore malloc\n");
                fclose(connfd);
                continue;
            }
            printf("[DEBUG] Allocato buffer di dimensione %ld\n", content_length);

            // b) Lettura del corpo della richiesta
            size_t bytes_read = fread(body, 1, content_length, connfd);
            printf("[DEBUG] Bytes letti da socket: %zu\n", bytes_read);

            // c) Individua in-memory l'inizio dei dati del file: dopo "\r\n\r\n"
            char *start = NULL;
            for (int i = 0; i + 4 < (int)bytes_read; i++) {
                if (body[i] == '\r' && body[i+1] == '\n' &&
                    body[i+2] == '\r' && body[i+3] == '\n') {
                    start = body + i + 4;
                    printf("[DEBUG] Inizio dati file a offset: %d\n", i+4);
                    break;
                }
            }
            if (!start) {
                start = body;
                printf("[DEBUG] Doppio CRLF non trovato, uso offset 0\n");
            }

            // d) Individua l'inizio del boundary finale: "--boundary"
            char fullBoundary[128];
            snprintf(fullBoundary, sizeof(fullBoundary), "--%s", boundary);
            char *end = NULL;
            int bLen = strlen(fullBoundary);
            printf("[DEBUG] Ricerca boundary finale '%s' di lunghezza %d\n", fullBoundary, bLen);
            for (int i = 0; i + bLen < (int)(bytes_read - (start - body)); i++) {
                if (memcmp(start + i, fullBoundary, bLen) == 0) {
                    end = start + i;
                    printf("[DEBUG] Boundary finale trovato a offset: %ld\n", start - body + i);
                    break;
                }
            }
            if (!end) {
                end = body + bytes_read;
                printf("[DEBUG] Boundary finale non trovato, uso end offset bytes_read=%zu\n", bytes_read);
            }

            // e) Estraggo il nome del file dal multipart: cerco "filename=\""
            char filename[256] = "upload.bin";
            char *fnHeader = strstr(body, "filename=\"");
            if (fnHeader) {
                fnHeader += 10;
                char *fnEnd = strchr(fnHeader, '"');
                if (fnEnd && fnEnd - fnHeader < (int)sizeof(filename)) {
                    int fnLen = fnEnd - fnHeader;
                    memcpy(filename, fnHeader, fnLen);
                    filename[fnLen] = '\0';
                }
                printf("[DEBUG] Nome file estratto: %s\n", filename);
            } else {
                printf("[DEBUG] Header filename non trovato, uso nome di fallback %s\n", filename);
            }

            // f) Apertura e scrittura su file binario
            FILE *file = fopen(filename, "wb");
            if (!file) {
                printf("[SERVER] Errore creazione file %s\n", filename);
                free(body);
                fclose(connfd);
                continue;
            }
            size_t written = fwrite(start, 1, end - start, file);
            fclose(file);
            printf("[DEBUG] Scritti %zu byte su file '%s'\n", written, filename);
            free(body);

            printf("[SERVER] Salvato file: %s (%zu byte)\n", filename, written);
        } else {
            printf("[DEBUG] Nessun upload gestito: metodo=%s, content_length=%ld\n", method, content_length);
        }

        // 8) Invio della risposta di conferma al browser
        printf("[DEBUG] Invio risposta HTML di conferma\n");
        fputs(HTMLResponse, connfd);
        fclose(connfd);

        printf("\n[SERVER] Sessione HTTP completata\n\n");
    }

    // 9) Chiusura del socket server (mai raggiunto)
    closeConnection(sockfd);
    return 0;
}
