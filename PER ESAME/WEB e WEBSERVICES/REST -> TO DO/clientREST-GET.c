// Ora visualizza **l’intero JSON** restituito dal server per l’endpoint /numeri-primi.
// Restano disponibili entrambe le funzioni:
//    • calcola-somma a b
//    • numeri-primi a b

/*
    SU WIRESHARK applico questo filtro: http.request.method == "GET" && tcp.port == 8000

    Posso lanciare:
        - ./client-get numeri-primi 0 30
        - ./client-get calcola-somma 10 30
*/

#include "network.h"

/********************* FUNC: calcolaSomma *************************/
static float calcolaSomma(float val1, float val2){
    char url[MTU];
    char response[MTU];

    snprintf(url, sizeof(url),
             "http://localhost:8000/calcola-somma?param1=%f&param2=%f",
             val1, val2);

    int status = doGET(url, response, sizeof(response));
    if(status != 200){
        fprintf(stderr, "[CLIENT] Errore HTTP: %d\n", status);
        return -1.0f;
    }

    /* Mostriamo il JSON ricevuto (opzionale) */
    // printf("JSON ricevuto: %s\n", response);

    float somma = 0.0f;
    if(sscanf(response, "{ \"somma\": %f", &somma) != 1){
        fprintf(stderr, "[CLIENT] Formato JSON inatteso: %s\n", response);
        return -1.0f;
    }
    return somma;
}

/********************* FUNC: primeNumber *************************/
/* Ora stampa anche il JSON completo prima di elaborarlo */
static int primeNumber(int a, int b){
    char url[MTU];
    char response[MTU];

    snprintf(url, sizeof(url),
             "http://localhost:8000/numeri-primi?param1=%d&param2=%d", a, b);

    int status = doGET(url, response, sizeof(response));
    if(status != 200){
        fprintf(stderr, "[CLIENT] Errore HTTP: %d\n", status);
        return -1;
    }

    /* --- stampa del JSON grezzo --- */
    printf("\nJSON numeri‑primi ricevuto:\n%s\n\n", response);

    int totale = 0;
    if(sscanf(response, "{ \"totale_numeri_primi\": %d", &totale) != 1){
        fprintf(stderr, "[CLIENT] Formato JSON inatteso o campo mancante.\n");
        return -1;
    }
    return totale;
}

/********************* MAIN *************************/
int main(int argc, char **argv){
    if(argc != 4){
        printf("USO: %s <calcola-somma|numeri-primi> op1 op2\n", argv[0]);
        return -1;
    }

    if(strcmp(argv[1], "calcola-somma") == 0){
        float a = atof(argv[2]);
        float b = atof(argv[3]);
        float risultato = calcolaSomma(a, b);
        if(risultato >= 0)
            printf("Somma = %.2f\n", risultato);
    }
    else if(strcmp(argv[1], "numeri-primi") == 0){
        int a = atoi(argv[2]);
        int b = atoi(argv[3]);
        int count = primeNumber(a, b);
        if(count == 0)
            printf("non ne ho trovati\n");
    }
    else {
        printf("Funzione sconosciuta: %s\n", argv[1]);
        return -1;
    }

    return 0;
}
