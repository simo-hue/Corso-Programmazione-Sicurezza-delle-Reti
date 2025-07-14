// Server HTTP molto semplice che espone due endpoint REST:
//   • /calcola-somma?param1=<a>&param2=<b>   → restituisce { "somma": a+b }

#include "network.h"

/* Ritorna la somma di due float */
static float calcolaSomma(float val1, float val2){
    return val1 + val2;
}

/***************** HELPER HTTP *****************/

/* Invia una risposta HTTP/1.1 con body JSON già pronto */
static void send_json(FILE *stream, int status_code, const char *body){
    const char *status_text =
        (status_code == 200 ? "OK" : (status_code == 404 ? "Not Found" : "Bad Request"));
    int len = (int)strlen(body);
    fprintf(stream,
            "HTTP/1.1 %d %s\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "\r\n"
            "%s",
            status_code, status_text, len, body);
}

/* Scarta il body in arrivo (per POST/PUT che non interessano al server) */
static void discard_body(FILE *stream, long length){
    for(long i = 0; i < length; ++i) fgetc(stream);
}

/***************** MAIN *****************/

int main(void){
    socketif_t sockfd = createTCPServer(8000);
    if(sockfd < 0){
        fprintf(stderr, "[SERVER] Errore creazione socket: %d\n", sockfd);
        return 1;
    }
    printf("[SERVER] In ascolto sulla porta 8000…\n");

    /* Server iterativo: un client alla volta, logica invariata */
    for(;;){
        FILE *connfd = acceptConnectionFD(sockfd);
        if(!connfd) continue;

        char line[MTU] = {0};
        char method[8] = {0};
        char url[MTU]  = {0};

        /**** 1ª riga della richiesta ****/
        if(!fgets(line, sizeof(line), connfd)){
            fclose(connfd);
            continue;
        }
        sscanf(line, "%7s %511s", method, url);   // es. "GET /calcola-somma?... HTTP/1.1"

        /**** Lettura header ****/
        long content_len = 0;
        while(fgets(line, sizeof(line), connfd)){
            if(strcmp(line, "\r\n") == 0) break;    // riga vuota → fine header
            if(strncmp(line, "Content-Length:", 15) == 0)
                content_len = atol(line + 15);
        }

        /* Se è POST/PUT scartiamo il body (qui non lo elaboriamo) */
        if(strcmp(method, "POST") == 0 || strcmp(method, "PUT") == 0)
            discard_body(connfd, content_len);

        /**** ROUTING ****/
        if(strstr(url, "calcola-somma")){
            /* parsing parametri ?param1=<a>&param2=<b>  */
            float a = 0.0f, b = 0.0f;
            if(sscanf(url, "%*[^?]?param1=%f&param2=%f", &a, &b) != 2){
                send_json(connfd, 400, "{ \"errore\": \"Parametri mancanti o malformati\" }\n");
            }else{
                float somma = calcolaSomma(a, b);

                char body[128];
                snprintf(body, sizeof(body), "{ \"somma\": %.2f }\n", somma);
                send_json(connfd, 200, body);
            }
        }else {
            /* Endpoint sconosciuto */
            send_json(connfd, 404, "{ \"errore\": \"Funzione non riconosciuta\" }\n");
        }

        fclose(connfd);
        printf("[SERVER] Connessione terminata.\n\n");
    }

    closeConnection(sockfd);
    return 0;
}
