#include "network.h"
#include <time.h>
#include <string.h>

// Funzione originale per il calcolo della somma
float calcolaSomma(float val1, float val2) {
    return (val1 + val2);
}

/**
 * Funzione per verificare se un numero è primo
 * @param n: numero da verificare
 * @return: 1 se primo, 0 se non primo
 */
int isPrime(int n) {
    // 0 e 1 non sono numeri primi
    if (n <= 1) return 0;
    
    // 2 è primo
    if (n == 2) return 1;
    
    // Numeri pari maggiori di 2 non sono primi
    if (n % 2 == 0) return 0;
    
    // Controlla divisibilità per numeri dispari fino a n/2
    for (int i = 3; i <= n / 2; i += 2) {
        if (n % i == 0) {
            return 0;
        }
    }
    
    return 1;
}

/**
 * Funzione per calcolare i numeri primi in formato legacy (compatibile con client esistente)
 * @param min: limite inferiore dell'intervallo
 * @param max: limite superiore dell'intervallo
 * @param response: buffer per la risposta JSON
 * @param execution_time: tempo di esecuzione in millisecondi
 */
void calcolaPrimiIntervalloLegacy(int min, int max, char* response, double execution_time) {
    char temp[1024];
    int count = 0;
    int first = 1;
    
    // Inizializza la risposta JSON in formato legacy
    sprintf(response, "{\n\"count\":");
    
    // Prima conta i numeri primi
    for (int i = min; i <= max; i++) {
        if (isPrime(i)) {
            count++;
        }
    }
    
    // Aggiunge il count
    sprintf(temp, "%d,\n\"intervallo\":\"[%d,%d]\",\n\"primi\":[", count, min, max);
    strcat(response, temp);
    
    // Aggiunge tutti i numeri primi
    for (int i = min; i <= max; i++) {
        if (isPrime(i)) {
            if (!first) {
                strcat(response, ",");
            }
            sprintf(temp, "%d", i);
            strcat(response, temp);
            first = 0;
        }
    }
    
    // Chiude l'array e aggiunge eventualmente il tempo
    if (execution_time > 0) {
        sprintf(temp, "],\n\"tempo_esecuzione_ms\":%.2f\n}", execution_time);
    } else {
        sprintf(temp, "]\n}");
    }
    strcat(response, temp);
}

/**
 * Funzione per calcolare i numeri primi in un intervallo e costruire la risposta JSON
 * @param min: limite inferiore dell'intervallo
 * @param max: limite superiore dell'intervallo
 * @param response: buffer per la risposta JSON
 * @param execution_time: tempo di esecuzione in millisecondi
 */
void calcolaPrimiIntervallo(int min, int max, char* response, double execution_time) {
    char temp[1024];
    int first = 1; // Flag per gestire la virgola nel JSON
    
    // Inizializza la risposta JSON
    sprintf(response, "{\n    \"intervallo\": {\"min\": %d, \"max\": %d},\n", min, max);
    strcat(response, "    \"numeri_primi\": [");
    
    // Trova tutti i numeri primi nell'intervallo
    for (int i = min; i <= max; i++) {
        if (isPrime(i)) {
            if (!first) {
                strcat(response, ", ");
            }
            sprintf(temp, "%d", i);
            strcat(response, temp);
            first = 0;
        }
    }
    
    // Chiude l'array e aggiunge il tempo di esecuzione
    sprintf(temp, "],\n    \"tempo_esecuzione_ms\": %.2f\n}", execution_time);
    strcat(response, temp);
}

/**
 * Funzione per parsare i parametri dall'URL per il servizio primi
 * @param url: URL della richiesta
 * @param min: puntatore per memorizzare il valore minimo
 * @param max: puntatore per memorizzare il valore massimo
 * @return: 1 se parsing riuscito, 0 altrimenti
 */
int parseParametriPrimi(char* url, int* min, int* max) {
    char *function, *param1, *param2;
    char url_copy[MTU];
    
    // Copia l'URL per non modificare l'originale
    strcpy(url_copy, url);
    
    // Parsing dei parametri: /calcola-primi?min=X&max=Y
    function = strtok(url_copy, "?&");
    param1 = strtok(NULL, "?&");
    param2 = strtok(NULL, "?&");
    
    if (param1 == NULL || param2 == NULL) {
        return 0;
    }
    
    // Parsing del primo parametro (min=X)
    char* key1 = strtok(param1, "=");
    char* val1 = strtok(NULL, "=");
    
    // Parsing del secondo parametro (max=Y)
    char* key2 = strtok(param2, "=");
    char* val2 = strtok(NULL, "=");
    
    if (val1 == NULL || val2 == NULL) {
        return 0;
    }
    
    // Assegna i valori verificando l'ordine dei parametri
    if (strcmp(key1, "min") == 0 && strcmp(key2, "max") == 0) {
        *min = atoi(val1);
        *max = atoi(val2);
    } else if (strcmp(key1, "max") == 0 && strcmp(key2, "min") == 0) {
        *min = atoi(val2);
        *max = atoi(val1);
    } else {
        return 0;
    }
    
    return 1;
}

int main() {
    socketif_t sockfd;
    FILE* connfd;
    int res, i;
    long length = 0;
    char request[MTU], url[MTU], method[10], c;
    
    // Creazione del server TCP sulla porta 8000
    sockfd = createTCPServer(8000);
    if (sockfd < 0) {
        printf("[SERVER] Errore nella creazione del server: %i\n", sockfd);
        return -1;
    }
    
    printf("[SERVER] Server HTTP avviato sulla porta 8000\n");
    printf("[SERVER] Servizi disponibili:\n");
    printf("         - GET /calcola-somma\n");
    printf("         - GET /numeri-primi\n\n");
    
    // Loop principale del server
    while(true) {
        // Accetta connessione dal client e ottiene file descriptor
        connfd = acceptConnectionFD(sockfd);
        printf("[SERVER] Nuova connessione accettata\n");
        
        // Legge la prima riga della richiesta HTTP (metodo, URL, versione)
        fgets(request, sizeof(request), connfd);
        strcpy(method, strtok(request, " "));
        strcpy(url, strtok(NULL, " "));
        
        printf("[SERVER] Richiesta: %s %s\n", method, url);
        
        // Legge gli header HTTP fino alla riga vuota
        while(request[0] != '\r') {
            fgets(request, sizeof(request), connfd);
            // Cerca l'header Content-Length per richieste POST
            if(strstr(request, "Content-Length:") != NULL) {
                length = atol(request + 15);
            }
        }
        
        // Se è una richiesta POST, legge il body (anche se non utilizzato)
        if(strcmp(method, "POST") == 0) {
            for(i = 0; i < length; i++) {
                c = fgetc(connfd);
            }
        }
        
        // *** ROUTING DEI SERVIZI ***
        
        // Servizio calcolo somma (supporta sia op1/op2 che param1/param2)
        if(strstr(url, "calcola-somma") != NULL) {
            printf("[SERVER] Chiamata al servizio calcola-somma\n");
            
            char *function, *param1, *param2;
            float somma, val1, val2;
            char url_copy[MTU];
            strcpy(url_copy, url);
   
            // Skeleton: decodifica (de-serializzazione) dei parametri
            function = strtok(url_copy, "?&");
            param1 = strtok(NULL, "?&");
            param2 = strtok(NULL, "?&");
            
            if (param1 != NULL && param2 != NULL) {
                // Parsing flessibile per supportare sia op1/op2 che param1/param2
                char* key1 = strtok(param1, "=");
                char* value1 = strtok(NULL, "=");
                char* key2 = strtok(param2, "=");
                char* value2 = strtok(NULL, "=");
                
                if (value1 != NULL && value2 != NULL) {
                    // Determina quale parametro è quale basandosi sul nome
                    if ((strcmp(key1, "op1") == 0 || strcmp(key1, "param1") == 0) && 
                        (strcmp(key2, "op2") == 0 || strcmp(key2, "param2") == 0)) {
                        val1 = atof(value1);
                        val2 = atof(value2);
                    } else if ((strcmp(key1, "op2") == 0 || strcmp(key1, "param2") == 0) && 
                               (strcmp(key2, "op1") == 0 || strcmp(key2, "param1") == 0)) {
                        val1 = atof(value2);
                        val2 = atof(value1);
                    } else {
                        fprintf(connfd, "HTTP/1.1 400 Bad Request\r\n\r\n{\r\n    \"errore\": \"Parametri non validi\"\r\n}\r\n");
                        printf("[SERVER] Errore: parametri non riconosciuti\n");
                        fclose(connfd);
                        continue;
                    }
                    
                    // Chiamata alla business logic
                    somma = calcolaSomma(val1, val2);
                    
                    // Skeleton: codifica (serializzazione) del risultato
                    fprintf(connfd, "HTTP/1.1 200 OK\r\n\r\n{\r\n    \"somma\": %f\r\n}\r\n", somma);
                    
                    printf("[SERVER] Somma calcolata: %.2f + %.2f = %.2f\n", val1, val2, somma);
                } else {
                    fprintf(connfd, "HTTP/1.1 400 Bad Request\r\n\r\n{\r\n    \"errore\": \"Valori parametri mancanti\"\r\n}\r\n");
                    printf("[SERVER] Errore: valori parametri mancanti\n");
                }
            } else {
                fprintf(connfd, "HTTP/1.1 400 Bad Request\r\n\r\n{\r\n    \"errore\": \"Parametri mancanti\"\r\n}\r\n");
                printf("[SERVER] Errore: parametri mancanti\n");
            }
        }
        
        // Servizio calcolo numeri primi (supporta sia /calcola-primi che /numeri-primi)
        else if(strstr(url, "calcola-primi") != NULL || strstr(url, "numeri-primi") != NULL) {
            printf("[SERVER] Chiamata al servizio numeri primi\n");
            
            int min, max;
            char response[4096];
            int is_legacy_format = (strstr(url, "numeri-primi") != NULL);
            
            // Parsing dei parametri dall'URL
            if (parseParametriPrimi(url, &min, &max)) {
                // Validazione dei parametri
                if (min > max) {
                    int temp = min;
                    min = max;
                    max = temp;
                }
                
                printf("[SERVER] Calcolo primi nell'intervallo [%d, %d]\n", min, max);
                
                // Misurazione del tempo di esecuzione
                clock_t start_time = clock();
                
                if (is_legacy_format) {
                    // Formato compatibile con il client esistente
                    calcolaPrimiIntervalloLegacy(min, max, response, 0);
                } else {
                    // Nuovo formato esteso
                    calcolaPrimiIntervallo(min, max, response, 0);
                }
                
                clock_t end_time = clock();
                double execution_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000.0;
                
                // Aggiorna la risposta con il tempo reale
                if (is_legacy_format) {
                    calcolaPrimiIntervalloLegacy(min, max, response, execution_time);
                } else {
                    calcolaPrimiIntervallo(min, max, response, execution_time);
                }
                
                // Invia la risposta HTTP con il JSON dei risultati
                fprintf(connfd, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s\r\n", response);
                
                printf("[SERVER] Calcolo completato in %.2f ms\n", execution_time);
            } else {
                // Parametri non validi
                fprintf(connfd, "HTTP/1.1 400 Bad Request\r\n\r\n{\r\n    \"errore\": \"Parametri non validi. Utilizzare: ?min=X&max=Y\"\r\n}\r\n");
                printf("[SERVER] Errore: parametri non validi\n");
            }
        }
        
        // Servizio non riconosciuto
        else {
            fprintf(connfd, "HTTP/1.1 404 Not Found\r\n\r\n{\r\n    \"errore\": \"Servizio non riconosciuto!\"\r\n}\r\n");
            printf("[SERVER] Errore: servizio non riconosciuto\n");
        }
        
        // Chiude la connessione con il client
        fclose(connfd);
        printf("[SERVER] Sessione HTTP completata\n\n");
    }
    
    // Chiude il socket del server (mai raggiunto nel loop infinito)
    closeConnection(sockfd);
    return 0;
}