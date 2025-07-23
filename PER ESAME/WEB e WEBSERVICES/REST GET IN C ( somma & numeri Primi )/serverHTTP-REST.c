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

// Funzione per contare i numeri primi in un intervallo
int contaNumeroPrimi(int min, int max) {
    int count = 0;
    for (int i = min; i <= max; i++) {
        if (isPrime(i)) {
            count++;
        }
    }
    return count;
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
    
    //INFO:
    printf("[SERVER] Server avviato sulla porta 8000\n");
    printf("[SERVER] Servizi disponibili:\n");
    printf("  - /calcola-somma\n");
    printf("  - /numeri-primi\n\n\n");
    
    // Accetto richieste per sempre
    while(true) {
        connfd = acceptConnectionFD(sockfd);
        
        
        fgets(request, sizeof(request), connfd);
        strcpy(method,strtok(request, " "));
        strcpy(url,strtok(NULL, " "));
        
        printf("[SERVER] Richiesta ricevuta: %s %s\n", method, url);
        
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
            printf("[SERVER] Chiamata a funzione sommatrice\n");
            
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
            
            printf("[SERVER] Parametri: val1=%f, val2=%f\n", val1, val2);
            
            // chiamata alla business logic
            somma = calcolaSomma(val1, val2);
            
            // skeleton: codifica (serializzazione) del risultato
            fprintf(connfd,"HTTP/1.1 200 OK\r\n\r\n{\r\n    \"somma\":%f\r\n}\r\n", somma);
            printf("[SERVER] Risultato inviato: %f\n", somma);
        }
        // Gestione del nuovo servizio numeri-primi
        else if(strstr(url, "numeri-primi")!=NULL)  {
            printf("[SERVER] Chiamata a funzione conteggio numeri primi\n");
            
            char *function, *param1, *param2;
            int min, max, count;
   
            // skeleton: decodifica (de-serializzazione) dei parametri
            function = strtok(url, "?&");
            param1 = strtok(NULL, "?&");
            param2 = strtok(NULL, "?&");
            
            // Parsing dei parametri min e max
            strtok(param1,"=");
            min = atoi(strtok(NULL,"="));
            strtok(param2,"=");
            max = atoi(strtok(NULL,"="));
            
            printf("[SERVER] Parametri: min=%d, max=%d\n", min, max);
            printf("[SERVER] Inizio calcolo numeri primi...\n");
            
            // chiamata alla business logic
            count = contaNumeroPrimi(min, max);
            
            printf("[SERVER] Calcolo completato. Numeri primi trovati: %d\n", count);
            
            // skeleton: codifica (serializzazione) del risultato
            fprintf(connfd,"HTTP/1.1 200 OK\r\n\r\n{\r\n    \"numeri_primi\":%d,\r\n    \"intervallo\":\"[%d,%d]\"\r\n}\r\n", count, min, max);
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