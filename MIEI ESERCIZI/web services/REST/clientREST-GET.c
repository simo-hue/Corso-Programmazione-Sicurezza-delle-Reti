#include "network.h"

float calcolaSomma(float val1, float val2)  {
    char request[MTU], response[MTU];
    
    // stub: codifica (serializzazione) dei parametri
    sprintf(request, "http://localhost:8000/calcola-somma?param1=%f&param2=%f", val1, val2);
    
    // chiamata del webservice
    int res = doGET(request, response, MTU);
    if (res < 0){
        printf("Errore: %i\n", res);
        return -1;
    }
    
    printf("Risposta dal server:\n%s\n", response);
    
    // stub: de-codifica (de-serializzazione) del risultato
    return atof(strstr(response,":")+1);
}

int main(int argc, char **argv){
    
    if(argc < 4)    {
        printf("USAGE: %s tipofunzione op1 op2\n", argv[0]);
        return -1;
    }
    else if(strcmp(argv[1],"calcola-somma")==0) {
        printf("Risultato: %f\n", calcolaSomma(atof(argv[2]), atof(argv[3])));
    }
    
    return 0;
}
