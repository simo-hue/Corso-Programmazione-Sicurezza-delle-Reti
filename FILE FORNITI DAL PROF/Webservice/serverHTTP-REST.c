#include "network.h"
#include <sys/wait.h>

float calcolaSomma(float val1, float val2)  {
   return (val1 + val2);
}

char* numeriPrimi(int min, int max, FILE* connfd){
    printf("Chiamata a funzione numeri primi con intervallo [%d, %d]\n", min, max);

    // Buffer iniziale per contenere la stringa JSON
    size_t size = 8192;
    char* json = malloc(size);
    if (!json) return NULL;

    strcpy(json, "{\n    \"numeri_primi\": [");
    int first = 1;
    char numbuf[32];

    for (int n = min; n <= max; n++) {
        if (n < 2) continue;
        int primo = 1;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                primo = 0;
                break;
            }
        }
        if (primo) {
            if (!first) strcat(json, ", ");
            snprintf(numbuf, sizeof(numbuf), "%d", n);
            strcat(json, numbuf);
            first = 0;
        }
    }

    strcat(json, "]\n}\n");

    // Invia anche la risposta HTTP al client
    fprintf(connfd, "HTTP/1.1 200 OK\r\n\r\n%s", json);

    return json;
}

int main(){
    socketif_t sockfd;
    FILE* connfd;
    int res, i;
    long length=0;
    char request[MTU], url[MTU], method[10], c;
    
    sockfd = createTCPServer(8000);
    if (sockfd < 0){
        printf("[SERVER] Errore: %i\n", sockfd);
        return -1;
    }
    
    printf("[SERVER] Server REST avviato sulla porta 8000\n");
    printf("[SERVER] Servizi disponibili:\n");
    printf("  - GET /calcola-somma?param1=X&param2=Y\n");
    printf("  - GET /numeri-primi?param1=MIN&param2=MAX\n\n");
    
    while(true) {
        connfd = acceptConnectionFD(sockfd);
        
        fgets(request, sizeof(request), connfd);
        strcpy(method,strtok(request, " "));
        strcpy(url,strtok(NULL, " "));
        while(request[0]!='\r') {
            fgets(request, sizeof(request), connfd);
            if(strstr(request, "Content-Length:")!=NULL)  {
                length = atol(request+15);
            }
        }
        
        if(strcmp(method, "POST")==0)  {
            for(i=0; i<length; i++)  {
                c = fgetc(connfd);
            }
        }
        
        // Gestione del servizio calcola-somma
        if(strstr(url, "calcola-somma")!=NULL)  {
            printf("Chiamata a funzione sommatrice\n");
            
            char *function, *op1, *op2;
            float somma, val1, val2;
   
            // skeleton: decodifica (de-serializzazione) dei parametri
            function = strtok(url, "?&");
            op1 = strtok(NULL, "?&");
            op2 = strtok(NULL, "?&");
            strtok(op1,"=");
            val1 = atof(strtok(NULL,"="));
            strtok(op2,"=");
            val2 = atof(strtok(NULL,"="));
            
            // chiamata alla business logic
            somma = calcolaSomma(val1, val2);
            
            // skeleton: codifica (serializzazione) del risultato
            fprintf(connfd,"HTTP/1.1 200 OK\r\n\r\n{\r\n    \"somma\":%f\r\n}\r\n", somma);
        }
        // Gestione del servizio numeri-primi
        else if(strstr(url, "numeri-primi")!=NULL)  {
            printf("Chiamata a funzione numeri primi\n");
            
            char *function, *op1, *op2;
            int min, max;
   
            // skeleton: decodifica (de-serializzazione) dei parametri
            function = strtok(url, "?&");
            op1 = strtok(NULL, "?&");
            op2 = strtok(NULL, "?&");
            strtok(op1,"=");
            min = atoi(strtok(NULL,"="));
            strtok(op2,"=");
            max = atoi(strtok(NULL,"="));
            
            // chiamata alla business logic
            struct timespec start, end;
            clock_gettime(CLOCK_REALTIME, &start);
            numeriPrimi(min, max, connfd);
            // Calcolo della differenza in secondi
            clock_gettime(CLOCK_REALTIME, &end);
            double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
            // Stampa del risultato
            printf("Tempo impiegato per il calcolo: %f secondi\n", time_spent);
        }
        else {
            fprintf(connfd,"HTTP/1.1 404 Not Found\r\n\r\n{\r\n    \"errore\": \"Funzione non riconosciuta!\",\r\n    \"servizi_disponibili\": [\"calcola-somma\", \"numeri-primi\"]\r\n}\r\n");
        }
        
        fclose(connfd);
                
        printf("\n\n[SERVER] sessione HTTP completata\n\n");
    }
    
    closeConnection(sockfd);
    return 0;
}