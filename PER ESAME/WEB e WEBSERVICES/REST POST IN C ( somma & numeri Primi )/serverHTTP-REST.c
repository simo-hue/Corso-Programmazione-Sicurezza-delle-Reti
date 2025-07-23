#include "network.h"

float calcolaSomma(float val1, float val2)  {
   return (val1 + val2);
}

// Funzione per verificare se un numero è primo
int isPrime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    
    // Ottimizzazione: controlla solo i divisori della forma 6k±1
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }
    return 1;
}

// Funzione per trovare i numeri primi in un intervallo e restituire sia il conteggio che la lista
int trovaNumeroPrimi(int min, int max, int *lista_primi, int max_size) {
    int count = 0;
    for (int i = min; i <= max && count < max_size; i++) {
        if (isPrime(i)) {
            lista_primi[count] = i;
            count++;
        }
    }
    return count;
}

// Funzione per estrarre un valore float da JSON
float extractFloatFromJSON(char* json, char* key) {
    char search_key[100];
    sprintf(search_key, "\"%s\":", key);
    char* pos = strstr(json, search_key);
    if (pos != NULL) {
        pos += strlen(search_key);
        // Salta eventuali spazi
        while (*pos == ' ' || *pos == '\t') pos++;
        return atof(pos);
    }
    return 0.0;
}

// Funzione per estrarre un valore int da JSON
int extractIntFromJSON(char* json, char* key) {
    char search_key[100];
    sprintf(search_key, "\"%s\":", key);
    char* pos = strstr(json, search_key);
    if (pos != NULL) {
        pos += strlen(search_key);
        // Salta eventuali spazi
        while (*pos == ' ' || *pos == '\t') pos++;
        return atoi(pos);
    }
    return 0;
}

int main(){
    socketif_t sockfd;
    FILE* connfd;
    int res, i;
    long length=0;
    char request[MTU], url[MTU], method[10], c;
    char body[MTU];
    
    sockfd = createTCPServer(8000);
    if (sockfd < 0){
        printf("[SERVER] Errore: %i\n", sockfd);
        return -1;
    }
    
    //INFO:
    printf("[SERVER] Server avviato sulla porta 8000\n");
    printf("[SERVER] Servizi disponibili:\n");
    printf("  - GET/POST /calcola-somma\n");
    printf("  - GET/POST /numeri-primi\n\n\n");
    
    // Accetto richieste per sempre
    while(true) {
        connfd = acceptConnectionFD(sockfd);
        
        fgets(request, sizeof(request), connfd);
        strcpy(method,strtok(request, " "));
        strcpy(url,strtok(NULL, " "));
        
        printf("[SERVER] Richiesta ricevuta: %s %s\n", method, url);
        
        // Inizializza length e body
        length = 0;
        memset(body, 0, sizeof(body));
        
        // Leggi gli header
        while(request[0]!='\r') {
            fgets(request, sizeof(request), connfd);
            if(strstr(request, "Content-Length:")!=NULL)  {
                length = atol(request+15);
            }
        }
        
        // Se è una POST, leggi il body
        if(strcmp(method, "POST")==0 && length > 0)  {
            for(i=0; i<length && i<MTU-1; i++)  {
                c = fgetc(connfd);
                body[i] = c;
            }
            body[i] = '\0';
            printf("[SERVER] Body ricevuto: %s\n", body);
        }
        
        // Gestione del servizio calcola-somma
        if(strstr(url, "calcola-somma")!=NULL)  {
            printf("[SERVER] Chiamata a funzione sommatrice\n");
            
            float val1, val2, somma;
            
            if(strcmp(method, "GET")==0) {
                // Parsing GET (parametri URL)
                char *function, *op1, *op2;
                function = strtok(url, "?&");
                op1 = strtok(NULL, "?&");
                op2 = strtok(NULL, "?&");
                strtok(op1,"=");
                val1 = atof(strtok(NULL,"="));
                strtok(op2,"=");
                val2 = atof(strtok(NULL,"="));
            } else {
                // Parsing POST (body JSON)
                val1 = extractFloatFromJSON(body, "param1");
                val2 = extractFloatFromJSON(body, "param2");
            }
            
            printf("[SERVER] Parametri: val1=%f, val2=%f\n", val1, val2);
            
            // chiamata alla business logic
            somma = calcolaSomma(val1, val2);
            
            // skeleton: codifica (serializzazione) del risultato
            fprintf(connfd,"HTTP/1.1 200 OK\r\n\r\n{\r\n    \"somma\":%f\r\n}\r\n", somma);
            printf("[SERVER] Risultato inviato: %f\n", somma);
        }
        // Gestione del servizio numeri-primi
        else if(strstr(url, "numeri-primi")!=NULL)  {
            printf("[SERVER] Chiamata a funzione ricerca numeri primi\n");
            
            int min, max, count;
            int lista_primi[1000]; // Array per contenere i numeri primi (max 1000)
            
            if(strcmp(method, "GET")==0) {
                // Parsing GET (parametri URL)
                char *function, *param1, *param2;
                function = strtok(url, "?&");
                param1 = strtok(NULL, "?&");
                param2 = strtok(NULL, "?&");
                
                // Parsing dei parametri min e max
                strtok(param1,"=");
                min = atoi(strtok(NULL,"="));
                strtok(param2,"=");
                max = atoi(strtok(NULL,"="));
            } else {
                // Parsing POST (body JSON)
                min = extractIntFromJSON(body, "min");
                max = extractIntFromJSON(body, "max");
            }
            
            printf("[SERVER] Parametri: min=%d, max=%d\n", min, max);
            printf("[SERVER] Inizio ricerca numeri primi...\n");
            
            // chiamata alla business logic
            count = trovaNumeroPrimi(min, max, lista_primi, 1000);
            
            printf("[SERVER] Ricerca completata. Numeri primi trovati: %d\n", count);
            
            // skeleton: codifica (serializzazione) del risultato con lista
            fprintf(connfd,"HTTP/1.1 200 OK\r\n\r\n{\r\n    \"count\":%d,\r\n    \"intervallo\":\"[%d,%d]\",\r\n    \"primi\":[", count, min, max);
            
            // Aggiungi la lista dei numeri primi
            for(int j = 0; j < count; j++) {
                if(j > 0) fprintf(connfd, ",");
                fprintf(connfd, "%d", lista_primi[j]);
            }
            
            fprintf(connfd, "]\r\n}\r\n");
        }
        else {
            printf("[SERVER] Funzione non riconosciuta: %s\n", url);
            fprintf(connfd,"HTTP/1.1 404 Not Found\r\n\r\n{\r\n    \"errore\":\"Funzione non riconosciuta!\",\r\n    \"servizi_disponibili\":[\"calcola-somma\", \"numeri-primi\"]\r\n}\r\n");
        }
        
        fclose(connfd);
                
        printf("[SERVER] Sessione HTTP completata\n\n");
    }
    
    closeConnection(sockfd);
    return 0;
}