// Eseguendo http://127.0.0.1:8000/ o su localhost NON cambia NULLA perchè entrambi puntano all’interfaccia di loopback 
// Se provo ad aprire un ALTRO server NON FUNZIONA, perchè NON posso avere 2 processi sulla stessa PORTA dello Stesso dispositivo
// PER WIRESHARK applico questo filtro: http && tcp.port == 8000

// Questo Server Accetta tramite URL La lettura di una risorsa

#include "network.h"

int main(){
    // Potrebbe essere una risposta predefinita
    // char *pathResponse = "/Users/simo/Downloads/mia_pagina.html";
    
    socketif_t sockfd;
    FILE* connfd;
    int res, i;
    long length=0;
    char request[MTU], method[10], c;
    
    // Mi metto a disposizione per connessioni TCP su porta 8000
    sockfd = createTCPServer(8000);
    if (sockfd < 0){
        printf("[SERVER] Errore: %i\n", sockfd);
        return -1;
    }
    
    while(true) { // una volta risposto chiudo la connessione ma rimango in ascolto per nuove connessioni
        connfd = acceptConnectionFD(sockfd);
        
        // Ricevo e stampo la richiesta:
        fgets(request, sizeof(request), connfd);
        printf("%s", request);

        // Inizio a costruire URL corretto della risorsa richiesta
        strcpy(method,strtok(request," "));
        char *url_path = strtok(NULL, " ");
        if (url_path[0] == '/') url_path++; // rimuove lo slash iniziale
        char pathResponse[1024];
        snprintf(pathResponse, sizeof(pathResponse), "/Users/simo/Downloads/%s", url_path);

        // Faccio parsing degli header HTTP
        // Continua a leggere finché non trova una riga vuota (\r\n), che separa header da corpo.
        while(request[0]!='\r') {
            fgets(request, sizeof(request), connfd);
            printf("%s", request);
            if(strstr(request, "Content-Length:")!=NULL)  {
                length = atol(request+15);
                //printf("length %ld\n", length);
            }
        }
        
        if(strcmp(method, "POST")==0)  {
            for(i=0; i<length; i++)  {
                c = fgetc(connfd);
                printf("%c", c);
            }
        }
        
        // Leggo il FILE
        FILE *html = fopen(pathResponse, "r");

        // Provo ad aprire da BROWSER il FILE
        if (html != NULL) { // Se esiste
            // Invio intestazione risposta valida
            fputs("HTTP/1.1 200 OK\r\n", connfd);
            fputs("Content-Type: text/html\r\n\r\n", connfd);

            // Invio carattere per carattere il contenuto del file
            while ((c = fgetc(html)) != EOF) {
                fputc(c, connfd);
            }
            fclose(html);
        } else { // se non esiste
            // Allora restituisco un errore
            fputs("HTTP/1.1 404 Not Found\r\n\r\n", connfd);
            fputs("<html><body><h1>File non trovato</h1></body></html>", connfd);
        }
        fclose(connfd);
                
        printf("\n\n[SERVER] sessione HTTP completata\n\n");
    }
    
    closeConnection(sockfd);
    return 0;
}

