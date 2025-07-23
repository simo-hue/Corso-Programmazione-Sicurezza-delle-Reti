/*
    SU WIRESHARK applico questo filtro: http.request.method == "POST" && tcp.port == 8000

    Posso lanciare:
        - ./client-post numeri-primi 0 30
        - ./client-post calcola-somma 10 30
*/

#include "network.h"

/*********************** POST D'ORDINE DEMO ************************/ 
static void demoPostOrdine(void){
    char response[MTU];
    const char *body =
        "{\r\n  \"Id\": 12345,\r\n  \"Customer\": \"John Smith\",\r\n  \"Quantity\": 1,\r\n  \"Price\": 10.00\r\n}";

    int status = doPOST("http://localhost:8000/prova", (char *)body, response, MTU);
    printf("Status code: %d\n", status);
    printf("Risposta dal server (raw): %s\n", response);
}

/*********************** POST NUMERI PRIMI (Implementazione manuale) *************************/
static void demoPostPrimes(int a, int b){
    char response[2000];
    char body[MTU];
    char http_request[MTU];

    // Creiamo il body JSON con i parametri
    snprintf(body, sizeof(body),
             "{\r\n  \"min\": %d,\r\n  \"max\": %d\r\n}", a, b);

    printf("Invio richiesta POST per numeri primi con body:\n%s\n", body);

    // Implementazione manuale della richiesta HTTP POST
    socketif_t sockfd = createTCPConnection("localhost", 8000);
    if (sockfd < 0) {
        printf("ERRORE: Impossibile connettersi al server (codice: %d)\n", sockfd);
        return;
    }

    // Creiamo la richiesta HTTP POST manualmente
    snprintf(http_request, sizeof(http_request),
        "POST /numeri-primi HTTP/1.1\r\n"
        "Host: localhost:8000\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s", (int)strlen(body), body);

    // Inviamo la richiesta
    int sent = TCPSend(sockfd, http_request, strlen(http_request));
    if (sent < 0) {
        printf("ERRORE: Impossibile inviare la richiesta\n");
        closeConnection(sockfd);
        return;
    }

    // Riceviamo la risposta
    memset(response, 0, sizeof(response));
    int received = TCPReceive(sockfd, response, sizeof(response)-1);
    if (received < 0) {
        printf("ERRORE: Impossibile ricevere la risposta\n");
        closeConnection(sockfd);
        return;
    }

    response[received] = '\0';
    closeConnection(sockfd);

    printf("Risposta completa ricevuta (%d bytes):\n%s\n", received, response);

    // Parsing della risposta HTTP per estrarre il body
    char* body_start = strstr(response, "\r\n\r\n");
    if (body_start == NULL) {
        printf("ERRORE: Formato risposta HTTP non valido\n");
        return;
    }
    body_start += 4; // Salta "\r\n\r\n"

    printf("JSON numeri-primi ricevuto:\n%s\n", body_start);

    // Parsing della risposta per estrarre count e lista
    char* count_pos = strstr(body_start, "\"count\":");
    if (count_pos != NULL) {
        int count = atoi(count_pos + 8);
        printf("Numeri primi trovati: %d\n", count);
        
        // Trova e stampa la lista dei numeri primi
        char* primi_start = strstr(body_start, "\"primi\":[");
        if (primi_start != NULL) {
            primi_start += 9; // Salta "\"primi\":["
            char* primi_end = strstr(primi_start, "]");
            
            if (primi_end != NULL) {
                // Crea una copia della stringa contenente la lista
                int list_len = primi_end - primi_start;
                if (list_len > 0 && list_len < 1900) {
                    char primi_list[2000];
                    strncpy(primi_list, primi_start, list_len);
                    primi_list[list_len] = '\0';
                    printf("Lista dei numeri primi: [%s]\n", primi_list);
                }
            }
        }
    } else {
        printf("Errore nel parsing della risposta\n");
    }
}

/*********************** GET CALCOLA SOMMA ************************/ 
static void demoGetSum(float a, float b){
    char url[MTU];
    char response[MTU];

    snprintf(url, sizeof(url),
             "http://localhost:8000/calcola-somma?param1=%f&param2=%f", a, b);

    int status = doGET(url, response, sizeof(response));
    //printf("Status code: %d\n", status);
    //printf("JSON calcola-somma ricevuto:\n%s\n", response);

    float somma = 0.0f;
    if(sscanf(response, "{ \"somma\": %f", &somma) == 1)
        printf("Somma = %.2f\n", somma);
}

/*********************** POST CALCOLA SOMMA (Implementazione manuale) ***********************/
static void demoPostSum(float a, float b){
    char response[MTU];
    char body[MTU];
    char http_request[MTU];

    // Creiamo il body JSON con i parametri
    snprintf(body, sizeof(body),
             "{\r\n  \"param1\": %f,\r\n  \"param2\": %f\r\n}", a, b);

    printf("Invio richiesta POST per calcola-somma con body:\n%s\n", body);

    // Implementazione manuale della richiesta HTTP POST
    socketif_t sockfd = createTCPConnection("localhost", 8000);
    if (sockfd < 0) {
        printf("ERRORE: Impossibile connettersi al server (codice: %d)\n", sockfd);
        return;
    }

    // Creiamo la richiesta HTTP POST manualmente
    snprintf(http_request, sizeof(http_request),
        "POST /calcola-somma HTTP/1.1\r\n"
        "Host: localhost:8000\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s", (int)strlen(body), body);

    // Inviamo la richiesta
    int sent = TCPSend(sockfd, http_request, strlen(http_request));
    if (sent < 0) {
        printf("ERRORE: Impossibile inviare la richiesta\n");
        closeConnection(sockfd);
        return;
    }

    // Riceviamo la risposta
    memset(response, 0, sizeof(response));
    int received = TCPReceive(sockfd, response, sizeof(response)-1);
    if (received < 0) {
        printf("ERRORE: Impossibile ricevere la risposta\n");
        closeConnection(sockfd);
        return;
    }

    response[received] = '\0';
    closeConnection(sockfd);

    // Parsing della risposta HTTP per estrarre il body
    char* body_start = strstr(response, "\r\n\r\n");
    if (body_start == NULL) {
        printf("ERRORE: Formato risposta HTTP non valido\n");
        return;
    }
    body_start += 4; // Salta "\r\n\r\n"

    printf("JSON calcola-somma ricevuto:\n%s\n", body_start);

    float somma = 0.0f;
    if(sscanf(body_start, "{ \"somma\": %f", &somma) == 1)
        printf("Somma = %.2f\n", somma);
}

/*************************** MAIN *********************************/
int main(int argc, char **argv){
    if(argc == 1){
        demoPostOrdine();
        return 0;
    }

    if(argc == 4 && strcmp(argv[1], "numeri-primi") == 0){
        int a = atoi(argv[2]);
        int b = atoi(argv[3]);
        demoPostPrimes(a, b);
        return 0;
    }

    if(argc == 4 && strcmp(argv[1], "calcola-somma") == 0){
        float a = atof(argv[2]);
        float b = atof(argv[3]);
        demoPostSum(a, b); // Ora usa POST invece di GET
        return 0;
    }

    printf("USO:\n  %s                       # POST ordine demo\n  %s numeri-primi a b       # POST numeri primi\n  %s calcola-somma a b      # POST calcola somma\n", argv[0], argv[0], argv[0]);
    return -1;
}