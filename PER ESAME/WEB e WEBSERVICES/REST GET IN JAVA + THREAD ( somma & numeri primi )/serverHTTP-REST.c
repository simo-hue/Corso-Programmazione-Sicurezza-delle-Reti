/*

    LANCIARE 3 SERVER: 
                        - ./srv 8000
                        - ./srv 8001
                        - ./srv 8002

    Fare partire il client in JAVA


    su WIRESHARK:
                  - http.request.method == "GET" && tcp.port == 8000 -> Solo richiesta
                  - http && tcp.port == 8000 -> vedere anche la risposta

*/


#include "network.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

float calcolaSomma(float val1, float val2)  {
   return (val1 + val2);
}

int isPrime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }
    return 1;
}

int trovaNumeroPrimi(int min, int max, int *lista_primi, int max_size) {
    int count = 0;
    int saved = 0;
    
    for (int i = min; i <= max; i++) {
        if (isPrime(i)) {
            count++;  // Conta sempre
            if (saved < max_size) {  // Salva solo se c'è spazio
                lista_primi[saved++] = i;
            }
        }
    }
    return count;  // Ritorna il conteggio totale, non solo quelli salvati
}

float extractFloatFromJSON(char* json, char* key) {
    char search_key[100];
    sprintf(search_key, "\"%s\":", key);
    char* pos = strstr(json, search_key);
    if (pos != NULL) {
        pos += strlen(search_key);
        while (*pos == ' ' || *pos == '\t') pos++;
        return atof(pos);
    }
    return 0.0;
}

int extractIntFromJSON(char* json, char* key) {
    char search_key[100];
    sprintf(search_key, "\"%s\":", key);
    char* pos = strstr(json, search_key);
    if (pos != NULL) {
        pos += strlen(search_key);
        while (*pos == ' ' || *pos == '\t') pos++;
        return atoi(pos);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int porta = 8000;  // porta default
    if (argc > 1) {
        porta = atoi(argv[1]);
        printf("[SERVER] Avvio sulla porta %d\n", porta);
    } else {
        printf("[SERVER] Nessuna porta specificata, uso porta default %d\n", porta);
    }

    socketif_t sockfd;
    FILE* connfd;
    int res, i;
    long length=0;
    char request[MTU], url[MTU], method[10], c;
    char body[MTU];

    sockfd = createTCPServer(porta);
    if (sockfd < 0){
        printf("[SERVER] Errore: %i\n", sockfd);
        return -1;
    }

    printf("[SERVER] Server avviato sulla porta %d\n", porta);
    printf("  - GET/POST /calcola-somma\n");
    printf("  - GET/POST /numeri-primi\n\n");

    while(true) {
        connfd = acceptConnectionFD(sockfd);
        if (connfd == NULL) {
            printf("[SERVER] Errore nell'accettare connessione\n");
            continue;
        }

        // Leggi la richiesta HTTP
        if (fgets(request, sizeof(request), connfd) == NULL) {
            fclose(connfd);
            continue;
        }
        
        strcpy(method, strtok(request, " "));
        strcpy(url, strtok(NULL, " "));

        length = 0;
        memset(body, 0, sizeof(body));

        // Leggi gli header HTTP
        while(request[0]!='\r') {
            if (fgets(request, sizeof(request), connfd) == NULL) break;
            if(strstr(request, "Content-Length:")!=NULL)  {
                length = atol(request+15);
            }
        }

        // Leggi il body se presente
        if(strcmp(method, "POST")==0 && length > 0)  {
            for(i=0; i<length && i<MTU-1; i++)  {
                c = fgetc(connfd);
                if (c == EOF) break;
                body[i] = c;
            }
            body[i] = '\0';
        }

        // Servizio numeri-primi
        if(strstr(url, "numeri-primi")!=NULL)  {
            int min, max, count;
            // Buffer più grande per evitare overflow
            int lista_primi[10000];  // Aumentato da 1000 a 10000

            printf("[SERVER] Richiesta numeri-primi ricevuta\n");

            if(strcmp(method, "GET")==0) {
                char *function, *param1, *param2;
                function = strtok(url, "?&");
                param1 = strtok(NULL, "?&");
                param2 = strtok(NULL, "?&");

                if (param1 && param2) {
                    strtok(param1,"=");
                    min = atoi(strtok(NULL,"="));
                    strtok(param2,"=");
                    max = atoi(strtok(NULL,"="));
                } else {
                    min = 1; max = 100;  // valori default
                }
            } else {
                min = extractIntFromJSON(body, "min");
                max = extractIntFromJSON(body, "max");
            }

            printf("[SERVER] Calcolo primi nell'intervallo [%d, %d]\n", min, max);

            count = trovaNumeroPrimi(min, max, lista_primi, 10000);

            printf("[SERVER] Trovati %d numeri primi\n", count);

            // Invia risposta JSON
            fprintf(connfd,"HTTP/1.1 200 OK\r\n");
            fprintf(connfd,"Content-Type: application/json\r\n");
            fprintf(connfd,"Connection: close\r\n\r\n");
            fprintf(connfd,"{\r\n  \"count\":%d,\r\n  \"intervallo\":\"[%d,%d]\",\r\n  \"primi\":[", count, min, max);
            
            // Invia solo i primi che sono stati salvati nell'array (massimo 10000)
            int to_send = (count < 10000) ? count : 10000;
            for(int j = 0; j < to_send; j++) {
                if(j > 0) fprintf(connfd, ",");
                fprintf(connfd, "%d", lista_primi[j]);
            }
            fprintf(connfd, "]\r\n}\r\n");
            
        } 
        
        // Servizio calcola-somma
        else if(strstr(url, "calcola-somma")!=NULL) {
            float val1, val2, somma;
            
            printf("[SERVER] Richiesta calcola-somma ricevuta\n");
            
            if(strcmp(method, "GET")==0) {
                char *function, *op1, *op2;
                function = strtok(url, "?&");
                op1 = strtok(NULL, "?&");
                op2 = strtok(NULL, "?&");
                
                if (op1 && op2) {
                    strtok(op1,"=");
                    val1 = atof(strtok(NULL,"="));
                    strtok(op2,"=");
                    val2 = atof(strtok(NULL,"="));
                } else {
                    val1 = 0; val2 = 0;
                }
            } else {
                val1 = extractFloatFromJSON(body, "param1");
                val2 = extractFloatFromJSON(body, "param2");
            }

            somma = calcolaSomma(val1, val2);
            
            fprintf(connfd,"HTTP/1.1 200 OK\r\n");
            fprintf(connfd,"Content-Type: application/json\r\n");
            fprintf(connfd,"Connection: close\r\n\r\n");
            fprintf(connfd,"{\r\n  \"somma\":%f\r\n}\r\n", somma);
            
        } 
        
        // Servizio non riconosciuto
        else {
            fprintf(connfd,"HTTP/1.1 404 Not Found\r\n");
            fprintf(connfd,"Content-Type: application/json\r\n");
            fprintf(connfd,"Connection: close\r\n\r\n");
            fprintf(connfd,"{\r\n  \"errore\":\"Funzione non riconosciuta!\",\r\n  \"servizi_disponibili\":[\"calcola-somma\", \"numeri-primi\"]\r\n}\r\n");
        }

        fflush(connfd);  // Assicurati che i dati siano inviati
        fclose(connfd);
    }

    closeConnection(sockfd);
    return 0;
}