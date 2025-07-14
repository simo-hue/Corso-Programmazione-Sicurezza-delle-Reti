// Ora supporta tre modalità:
//   1) POST di un ordine demo (default senza argomenti)
//   2) GET numeri-primi a b   → stampa JSON + conteggio
//   3) GET calcola-somma a b   → stampa JSON + somma

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

/*********************** GET CALCOLA SOMMA ************************/ 
static void demoGetSum(float a, float b){
    char url[MTU];
    char response[MTU];

    snprintf(url, sizeof(url),
             "http://localhost:8000/calcola-somma?param1=%f&param2=%f", a, b);

    int status = doGET(url, response, sizeof(response));
    printf("Status code: %d\n", status);
    printf("JSON calcola-somma ricevuto:\n%s\n", response);

    float somma = 0.0f;
    if(sscanf(response, "{ \"somma\": %f", &somma) == 1)
        printf("Somma = %.2f\n", somma);
}

/*************************** MAIN *********************************/
int main(int argc, char **argv){
    if(argc == 1){
        demoPostOrdine();
        return 0;
    }
    
    if(argc == 4 && strcmp(argv[1], "calcola-somma") == 0){
        float a = atof(argv[2]);
        float b = atof(argv[3]);
        demoGetSum(a, b);
        return 0;
    }

    printf("USO:\n  %s                       # POST ordine demo\n  %s numeri-primi a b       # GET JSON primi\n  %s calcola-somma a b      # GET JSON somma\n", argv[0], argv[0], argv[0]);
    return -1;
}
