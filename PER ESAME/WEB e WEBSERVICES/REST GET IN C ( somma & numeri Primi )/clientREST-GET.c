/*
    SU WIRESHARK applico questo filtro: http.request.method == "GET" && tcp.port == 8000

    Posso lanciare:
        - ./client-get numeri-primi 0 30
        - ./client-get calcola-somma 10 30


            !!! RICHIESTE TROPPO AMPIE SUPERANO L'MTU !!!

    JSON DEL SERVER:
        {
        "count":8,
        "intervallo":"[2,20]",
        "primi":[2,3,5,7,11,13,17,19]
        }
*/
#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float calcolaSomma(float val1, float val2)  {
    char response[MTU];
    
    ////printf("DEBUG: Inizio funzione calcolaSomma\n");
    ////printf("DEBUG: val1=%f, val2=%f\n", val1, val2);
    
    // Inizializza il buffer
    memset(response, 0, MTU);
    
    // Creiamo la richiesta HTTP manualmente
    char path[MTU];
    sprintf(path, "/calcola-somma?param1=%f&param2=%f", val1, val2);
   
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
    
    ////printf("DEBUG: Richiesta HTTP:\n%s", http_request);
    
    // Inviamo la richiesta
    int sent = TCPSend(sockfd, http_request, strlen(http_request));
    if (sent < 0) {
        printf("ERRORE: Impossibile inviare la richiesta\n");
        closeConnection(sockfd);
        return -1;
    }
    
    ////printf("DEBUG: Richiesta inviata (%d bytes)\n", sent);
    
    // Riceviamo la risposta
    int received = TCPReceive(sockfd, response, MTU-1);
    if (received < 0) {
        printf("ERRORE: Impossibile ricevere la risposta\n");
        closeConnection(sockfd);
        return -1;
    }
    
    response[received] = '\0';
    ////printf("DEBUG: Risposta ricevuta (%d bytes):\n%s\n", received, response);
    
    closeConnection(sockfd);
    
    // Parsing della risposta
    char* body_start = strstr(response, "\r\n\r\n");
    if (body_start == NULL) {
        printf("ERRORE: Formato risposta HTTP non valido\n");
        return -1;
    }
    
    body_start += 4; // Salta "\r\n\r\n"
    ////printf("DEBUG: Body della risposta: %s\n", body_start);
    
    // Cerca il valore dopo i due punti
    char* colon_pos = strstr(body_start, ":");
    if (colon_pos == NULL) {
        printf("ERRORE: ':' non trovato nella risposta\n");
        return -1;
    }
    
    float result = atof(colon_pos + 1);
    ////printf("DEBUG: Risultato parsing: %f\n", result);
    
    return result;
}

int numeriPrimi(float a, float b){
    char response[MTU];
    int min = (int)a;
    int max = (int)b;
    
    //printf("DEBUG: Inizio funzione numeriPrimi\n");
    //printf("DEBUG: min=%d, max=%d\n", min, max);
    
    // Inizializza il buffer
    memset(response, 0, MTU);
    
    // Creiamo la richiesta HTTP per la funzione numeri-primi
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
    
    // Parsing del JSON per estrarre count e lista primi
    char* count_pos = strstr(body_start, "\"count\":");
    if (count_pos == NULL) {
        printf("ERRORE: 'count' non trovato nella risposta\n");
        return -1;
    }
    
    int count = atoi(count_pos + 8); // 8 = lunghezza di "\"count\":"
    
    // Trova e stampa la lista dei numeri primi
    char* primi_start = strstr(body_start, "\"primi\":[");
    if (primi_start != NULL) {
        primi_start += 9; // Salta "\"primi\":["
        char* primi_end = strstr(primi_start, "]");
        
        if (primi_end != NULL) {
            // Crea una copia della stringa contenente la lista
            int list_len = primi_end - primi_start;
            char primi_list[MTU];
            strncpy(primi_list, primi_start, list_len);
            primi_list[list_len] = '\0';
            
            printf("Lista dei numeri primi trovati: [%s]\n", primi_list);
        }
    }
    
    return count;
}

int main(int argc, char **argv){
    
    //printf("DEBUG: Inizio main con %d argomenti\n", argc);
    
    if(argc < 4)    {
        printf("USAGE: %s tipofunzione op1 op2\n", argv[0]);
        printf("Funzioni disponibili:\n");
        printf("  calcola-somma <val1> <val2>  - Calcola la somma di due numeri\n");
        printf("  numeri-primi <min> <max>     - Conta i numeri primi nell'intervallo [min,max]\n");
        return -1;
    }
    else if(strcmp(argv[1],"calcola-somma")==0) {
        //printf("DEBUG: Chiamando calcolaSomma\n");
        float risultato = calcolaSomma(atof(argv[2]), atof(argv[3]));
        //printf("DEBUG: calcolaSomma ritornata con: %f\n", risultato);
        
        if (risultato != -1) {
            printf("Risultato: %f\n", risultato);
        } else {
            printf("ERRORE: Operazione fallita\n");
        }
    }
    else if(strcmp(argv[1],"numeri-primi")==0) {
        //printf("DEBUG: Chiamando numeriPrimi\n");
        int risultato = numeriPrimi(atof(argv[2]), atof(argv[3]));
        //printf("DEBUG: numeriPrimi ritornata con: %d\n", risultato);
        
        if (risultato != -1) {
            printf("Numeri Primi nell'intervallo [%.0f,%.0f] sono: %d\n", 
                   atof(argv[2]), atof(argv[3]), risultato);
        } else {
            printf("ERRORE: Operazione fallita\n");
        }
    }
    else {
        printf("ERRORE: Funzione '%s' non riconosciuta\n", argv[1]);
        printf("Funzioni disponibili:\n");
        printf("  calcola-somma <val1> <val2>  - Calcola la somma di due numeri\n");
        printf("  numeri-primi <min> <max>     - Conta i numeri primi nell'intervallo [min,max]\n");
        return -1;
    }
    
    return 0;
}