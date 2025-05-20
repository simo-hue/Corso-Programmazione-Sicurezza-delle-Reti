// clientREST-GET.c – versione corretta (20 maggio 2025)
// Client da linea di comando per interrogare il nostro mini‑web‑service REST.
// Espone due funzioni:
//    • calcola-somma a b         → stampa la somma (float) di a+b
//    • numeri-primi a b          → conta i numeri primi nell’intervallo [a,b]

#include "network.h"

/********************* FUNC: calcolaSomma *************************/
/* Interroga l’endpoint /calcola-somma e ritorna la somma */
static float calcolaSomma(float val1, float val2){
    char url[MTU];                       // richiesta completa
    char response[MTU];                  // buffer risposta

    /* Costruiamo l’URL con i parametri, evitando overflow con snprintf */
    snprintf(url, sizeof(url),
             "http://localhost:8000/calcola-somma?param1=%f&param2=%f",
             val1, val2);

    /* Eseguiamo la GET → doGET ritorna lo status‑code HTTP */
    int status = doGET(url, response, sizeof(response));
    if(status != 200){
        fprintf(stderr, "[CLIENT] Errore HTTP: %d\n", status);
        return -1.0f;
    }

    /* Estraggo il campo JSON { "somma": <numero> } */
    float somma = 0.0f;
    if(sscanf(response, "{ \"somma\": %f", &somma) != 1){
        fprintf(stderr, "[CLIENT] Formato JSON inatteso: %s\n", response);
        return -1.0f;
    }
    return somma;
}

/********************* FUNC: primeNumber *************************/
/* Conta quanti numeri primi sono compresi tra a e b (inclusi) */
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

    int totale = 0;
    if(sscanf(response, "{ \"totale_numeri_primi\": %d", &totale) != 1){
        fprintf(stderr, "[CLIENT] Formato JSON inatteso: %s\n", response);
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

    /* prima arg = nome funzione */
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
        if(count >= 0)
            printf("Numeri primi in [%d, %d] sono %d\n", a, b, count);
    }
    else {
        printf("Funzione sconosciuta: %s\n", argv[1]);
        return -1;
    }

    return 0;
}
