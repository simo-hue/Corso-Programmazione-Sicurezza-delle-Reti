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
    for (int i = min; i <= max && count < max_size; i++) {
        if (isPrime(i)) {
            lista_primi[count++] = i;
        }
    }
    return count;
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
    int porta = 8000;
    if (argc > 1)
        porta = atoi(argv[1]);

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

        fgets(request, sizeof(request), connfd);
        strcpy(method, strtok(request, " "));
        strcpy(url, strtok(NULL, " "));

        length = 0;
        memset(body, 0, sizeof(body));

        while(request[0]!='\r') {
            fgets(request, sizeof(request), connfd);
            if(strstr(request, "Content-Length:")!=NULL)  {
                length = atol(request+15);
            }
        }

        if(strcmp(method, "POST")==0 && length > 0)  {
            for(i=0; i<length && i<MTU-1; i++)  {
                c = fgetc(connfd);
                body[i] = c;
            }
            body[i] = '\0';
        }

        if(strstr(url, "numeri-primi")!=NULL)  {
            int min, max, count;
            int lista_primi[1000];

            if(strcmp(method, "GET")==0) {
                char *function, *param1, *param2;
                function = strtok(url, "?&");
                param1 = strtok(NULL, "?&");
                param2 = strtok(NULL, "?&");

                strtok(param1,"=");
                min = atoi(strtok(NULL,"="));
                strtok(param2,"=");
                max = atoi(strtok(NULL,"="));
            } else {
                min = extractIntFromJSON(body, "min");
                max = extractIntFromJSON(body, "max");
            }

            count = trovaNumeroPrimi(min, max, lista_primi, 1000);

            fprintf(connfd,"HTTP/1.1 200 OK\r\n\r\n{\r\n  \"count\":%d,\r\n  \"intervallo\":\"[%d,%d]\",\r\n  \"primi\":[", count, min, max);
            for(int j = 0; j < count; j++) {
                if(j > 0) fprintf(connfd, ",");
                fprintf(connfd, "%d", lista_primi[j]);
            }
            fprintf(connfd, "]\r\n}\r\n");
        } else if(strstr(url, "calcola-somma")!=NULL) {
            float val1, val2, somma;
            if(strcmp(method, "GET")==0) {
                char *function, *op1, *op2;
                function = strtok(url, "?&");
                op1 = strtok(NULL, "?&");
                op2 = strtok(NULL, "?&");
                strtok(op1,"=");
                val1 = atof(strtok(NULL,"="));
                strtok(op2,"=");
                val2 = atof(strtok(NULL,"="));
            } else {
                val1 = extractFloatFromJSON(body, "param1");
                val2 = extractFloatFromJSON(body, "param2");
            }

            somma = calcolaSomma(val1, val2);
            fprintf(connfd,"HTTP/1.1 200 OK\r\n\r\n{\r\n  \"somma\":%f\r\n}\r\n", somma);
        } else {
            fprintf(connfd,"HTTP/1.1 404 Not Found\r\n\r\n{\r\n  \"errore\":\"Funzione non riconosciuta!\",\r\n  \"servizi_disponibili\":[\"calcola-somma\", \"numeri-primi\"]\r\n}\r\n");
        }

        fclose(connfd);
    }

    closeConnection(sockfd);
    return 0;
}