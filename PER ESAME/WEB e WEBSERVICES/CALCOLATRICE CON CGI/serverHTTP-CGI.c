// Richieste analizzabili tramite -> http.request.method == "GET" && tcp.port == 8000
// Ricordarsi che CHIARAMENTE sono sulla loopback

// Secondo parametro corrisponde al file descriptor dove scrivo la risposta ( in questo caso sulla connessione HTTP )
// Ovvero lo stream TCP associato alla connessione del client

#include "network.h"

/*
    eseguita lato server che elabora i parametri passati 
    e genera la pagina di risposta dinamica
*/
void sommatrice(char *query, FILE *out) {
    char buffer[MTU];
    char *op1 = NULL, *op2 = NULL;
    float val1 = 0, val2 = 0, somma = 0;

    // Creiamo una copia della query per non modificarne la memoria originale
    strncpy(buffer, query, MTU);
    buffer[MTU - 1] = '\0';  // per sicurezza

    // Parsing sicuro
    op1 = strtok(buffer, "&");
    op2 = strtok(NULL, "&");

    if (op1 && strstr(op1, "=")) {
        strtok(op1, "=");                // scarta "op1"
        char *valstr1 = strtok(NULL, "=");
        if (valstr1) val1 = atof(valstr1);
    }

    if (op2 && strstr(op2, "=")) {
        strtok(op2, "=");                // scarta "op2"
        char *valstr2 = strtok(NULL, "=");
        if (valstr2) val2 = atof(valstr2);
    }

    somma = val1 + val2;

    // Risposta HTTP risutante
    fprintf(out, "HTTP/1.1 200 OK\r\n\r\n"
                 "<html><head><title>Risultato</title></head>"
                 "<body>Risultato = %f</body></html>\r\n\r\n", somma);
}

int main(){
    socketif_t sockfd;
    FILE* connfd;
    int res, i;
    long length=0;
    char request[MTU], url[MTU], method[10], c;
    char body[MTU] = {0}; // sarà valorizzata a seconda del metodo
    char query[MTU] = {0}; // sarà valorizzata a seconda del metodo
    char *html="HTTP/1.1 200 OK\r\n\r\n<html><head><title>An Example Page</title></head><body>Hello World, this is a very simple HTML document.</body></html>\r\n\r\n";
    
    // Provo a creare il SERVER
    sockfd = createTCPServer(8000);
    if (sockfd < 0){
        printf("[SERVER] Errore: %i\n", sockfd);
        return -1;
    }
    
    // Continua ad accettare NUOVE CONNESSIONI PER SEMPRE
    while(true) {
        connfd = acceptConnectionFD(sockfd);
        
        fgets(request, sizeof(request), connfd);
        strcpy(method,strtok(request, " "));
        strcpy(url,strtok(NULL, " "));

        // reset variabili
        length = 0;
        body[0] = '\0';
        query[0] = '\0';

        // Leggi gli header per cercare "Content-Length"
        while(request[0]!='\r') {
            fgets(request, sizeof(request), connfd);
            if(strstr(request, "Content-Length:")!=NULL)  {
                length = atol(request+15);
            }
        }
        
        // Passo il BODY a sommatrice ( perchè li ci stanno i parametri ) -> Leggo il BODY
        if(strcmp(method, "POST")==0)  {

            printf("\n metodo POST identificato\n");

            char body[MTU] = {0};
            // Leggo il BODY
            for(i=0; i<length; i++) {
                body[i] = fgetc(connfd);
            }

            strcpy(query, body);  // passo il body alla funzione sommatrice
            printf("Metodo POST, body letto: %s\n", body);
        }
        // se è GET con parametri
        else if (strcmp(method, "GET") == 0 && strchr(url, '?') != NULL) {
            strcpy(query, strchr(url, '?') + 1);  // copia tutto dopo ?
            strtok(url, "?"); // rimuove i parametri da url
            printf("Metodo GET, parametri: %s\n", query);
        }

        // Gestione risposta
        if (strstr(url, "sommatrice") != NULL) {
            printf("Pagina dinamica\n");
            sommatrice(query, connfd);
        } else {
            printf("Pagina statica\n");
            fputs(html, connfd);
        }

        fclose(connfd);
        printf("[SERVER] sessione HTTP completata\n\n");
    }
    
    closeConnection(sockfd);
    return 0;
}



