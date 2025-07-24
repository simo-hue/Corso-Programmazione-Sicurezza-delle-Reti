/*
    Supporta entrambi i formati:
    - ./client-get calcola-somma 10 30
    - ./client-get numeri-primi 1 30
    
    Il server risponde con JSON nel formato:
    Somma: {"somma": 40.0}
    Primi : {"count":8, "intervallo":"[2,20]", "primi":[2,3,5,7,11,13,17,19]}
*/
#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Funzione per il calcolo della somma
 * Compatibile con il formato param1/param2 del client originale
 */
float calcolaSomma(float val1, float val2) {
    char response[MTU];
    
    //printf("DEBUG: Inizio funzione calcolaSomma\n");
    //printf("DEBUG: val1=%.2f, val2=%.2f\n", val1, val2);
    
    // Inizializza il buffer
    memset(response, 0, MTU);
    
    // Creiamo la richiesta HTTP manualmente usando param1/param2 per compatibilità
    char path[MTU];
    sprintf(path, "/calcola-somma?param1=%.2f&param2=%.2f", val1, val2);
   
    socketif_t sockfd = createTCPConnection("localhost", 8000);
    if (sockfd < 0) {
        printf("ERRORE: Impossibile connettersi al server (codice: %d)\n", sockfd);
        return -1;
    }
    
    // Creiamo la richiesta HTTP
    char http_request[MTU];
    sprintf(http_request, 
        "GET %s HTTP/1.1\r\n"
        "Host: localhost:8000\r\n"
        "Connection: close\r\n"
        "\r\n", 
        path);
    
    //printf("DEBUG: Richiesta HTTP:\n%s", http_request);
    
    // Inviamo la richiesta
    int sent = TCPSend(sockfd, http_request, strlen(http_request));
    if (sent < 0) {
        printf("ERRORE: Impossibile inviare la richiesta\n");
        closeConnection(sockfd);
        return -1;
    }
    
    //printf("DEBUG: Richiesta inviata (%d bytes)\n", sent);
    
    // Riceviamo la risposta
    int received = TCPReceive(sockfd, response, MTU-1);
    if (received < 0) {
        printf("ERRORE: Impossibile ricevere la risposta\n");
        closeConnection(sockfd);
        return -1;
    }
    
    response[received] = '\0';
    //printf("DEBUG: Risposta ricevuta (%d bytes):\n%s\n", received, response);
    
    closeConnection(sockfd);
    
    // Parsing della risposta
    char* body_start = strstr(response, "\r\n\r\n");
    if (body_start == NULL) {
        printf("ERRORE: Formato risposta HTTP non valido\n");
        return -1;
    }
    
    body_start += 4; // Salta "\r\n\r\n"
    //printf("DEBUG: Body della risposta: %s\n", body_start);
    
    // Cerca "somma": nel JSON
    char* somma_pos = strstr(body_start, "\"somma\":");
    if (somma_pos == NULL) {
        printf("ERRORE: 'somma' non trovato nella risposta\n");
        return -1;
    }
    
    // Trova il valore dopo i due punti
    char* colon_pos = strchr(somma_pos, ':');
    if (colon_pos == NULL) {
        printf("ERRORE: ':' non trovato dopo 'somma'\n");
        return -1;
    }
    
    float result = atof(colon_pos + 1);
    //printf("DEBUG: Risultato parsing: %.2f\n", result);
    
    return result;
}

/**
 * Funzione per il calcolo dei numeri primi
 * Usa il formato legacy /numeri-primi per compatibilità
 */
int numeriPrimi(float a, float b) {
    char response[MTU * 4]; // Buffer più grande per liste lunghe
    int min = (int)a;
    int max = (int)b;
    
    //printf("DEBUG: Inizio funzione numeriPrimi\n");
    //printf("DEBUG: min=%d, max=%d\n", min, max);
    
    // Inizializza il buffer
    memset(response, 0, sizeof(response));
    
    // Creiamo la richiesta HTTP per la funzione numeri-primi (formato legacy)
    char path[MTU];
    sprintf(path, "/numeri-primi?min=%d&max=%d", min, max);
    //printf("DEBUG: Path creato: %s\n", path);
    
    // Connessione al server
    //printf("DEBUG: Tentativo di connessione al server...\n");
    
    socketif_t sockfd = createTCPConnection("localhost", 8000);
    if (sockfd < 0) {
        printf("ERRORE: Impossibile connettersi al server (codice: %d)\n", sockfd);
        return -1;
    }
    
    //printf("DEBUG: Connessione stabilita\n");
    
    // Creiamo la richiesta HTTP
    char http_request[MTU];
    sprintf(http_request, 
        "GET %s HTTP/1.1\r\n"
        "Host: localhost:8000\r\n"
        "Connection: close\r\n"
        "\r\n", 
        path);
    
    //printf("DEBUG: Richiesta HTTP:\n%s", http_request);
    
    // Inviamo la richiesta
    int sent = TCPSend(sockfd, http_request, strlen(http_request));
    if (sent < 0) {
        printf("ERRORE: Impossibile inviare la richiesta\n");
        closeConnection(sockfd);
        return -1;
    }
    
    //printf("DEBUG: Richiesta inviata (%d bytes)\n", sent);
    
    // Riceviamo la risposta (potrebbe richiedere più chiamate per risposte grandi)
    int total_received = 0;
    int received;
    
    do {
        received = TCPReceive(sockfd, response + total_received, sizeof(response) - total_received - 1);
        if (received > 0) {
            total_received += received;
        }
    } while (received > 0 && total_received < sizeof(response) - 1);
    
    if (total_received <= 0) {
        printf("ERRORE: Impossibile ricevere la risposta\n");
        closeConnection(sockfd);
        return -1;
    }
    
    response[total_received] = '\0';
    //printf("DEBUG: Risposta ricevuta (%d bytes):\n%s\n", total_received, response);
    
    closeConnection(sockfd);
    
    // Parsing della risposta
    char* body_start = strstr(response, "\r\n\r\n");
    if (body_start == NULL) {
        printf("ERRORE: Formato risposta HTTP non valido\n");
        return -1;
    }
    
    body_start += 4; // Salta "\r\n\r\n"
    //printf("DEBUG: Body della risposta: %s\n", body_start);
    
    // Parsing del JSON per estrarre count
    char* count_pos = strstr(body_start, "\"count\":");
    int count = 0;
    if (count_pos != NULL) {
        count = atoi(count_pos + 8); // 8 = lunghezza di "\"count\":"
        //printf("DEBUG: Count estratto: %d\n", count);
    } else {
        printf("ERRORE: 'count' non trovato nella risposta\n");
        return -1;
    }
    
    // Trova e stampa la lista dei numeri primi
    char* primi_start = strstr(body_start, "\"primi\":[");
    if (primi_start != NULL) {
        primi_start += 9; // Salta "\"primi\":["
        char* primi_end = strstr(primi_start, "]");
        
        if (primi_end != NULL) {
            // Crea una copia della stringa contenente la lista
            int list_len = primi_end - primi_start;
            char primi_list[MTU * 2];
            strncpy(primi_list, primi_start, list_len);
            primi_list[list_len] = '\0';
            
            printf("Lista dei numeri primi trovati: [%s]\n", primi_list);
        }
    } else {
        printf("AVVISO: Lista primi non trovata nella risposta\n");
    }
    
    // Estrai tempo di esecuzione se presente
    char* tempo_pos = strstr(body_start, "\"tempo_esecuzione_ms\":");
    if (tempo_pos != NULL) {
        double tempo = atof(tempo_pos + 22); // 22 = lunghezza di "\"tempo_esecuzione_ms\":"
        printf("Tempo di esecuzione: %.2f ms\n", tempo);
    }
    
    return count;
}

int main(int argc, char **argv) {
    
    //printf("DEBUG: Inizio main con %d argomenti\n", argc);
    
    if(argc < 4) {
        printf("USAGE: %s tipofunzione op1 op2\n", argv[0]);
        printf("Funzioni disponibili:\n");
        printf("  calcola-somma <val1> <val2>  - Calcola la somma di due numeri\n");
        printf("  numeri-primi <min> <max>     - Trova i numeri primi nell'intervallo [min,max]\n");
        return -1;
    }
    else if(strcmp(argv[1], "calcola-somma") == 0) {
        //printf("DEBUG: Chiamando calcolaSomma\n");
        float risultato = calcolaSomma(atof(argv[2]), atof(argv[3]));
        //printf("DEBUG: calcolaSomma ritornata con: %.2f\n", risultato);
        
        if (risultato != -1) {
            printf("Risultato somma: %.2f\n", risultato);
        } else {
            printf("ERRORE: Operazione fallita\n");
        }
    }
    else if(strcmp(argv[1], "numeri-primi") == 0) {
        //printf("DEBUG: Chiamando numeriPrimi\n");
        int risultato = numeriPrimi(atof(argv[2]), atof(argv[3]));
        //printf("DEBUG: numeriPrimi ritornata con: %d\n", risultato);
        
        if (risultato != -1) {
            printf("Numero totale di primi nell'intervallo [%.0f,%.0f]: %d\n", 
                   atof(argv[2]), atof(argv[3]), risultato);
        } else {
            printf("ERRORE: Operazione fallita\n");
        }
    }
    else {
        printf("ERRORE: Funzione '%s' non riconosciuta\n", argv[1]);
        printf("Funzioni disponibili:\n");
        printf("  calcola-somma <val1> <val2>  - Calcola la somma di due numeri\n");
        printf("  numeri-primi <min> <max>     - Trova i numeri primi nell'intervallo [min,max]\n");
        return -1;
    }
    
    return 0;
}