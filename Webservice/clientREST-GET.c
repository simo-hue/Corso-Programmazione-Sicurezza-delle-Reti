#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* returns the sum via remote call; on error returns NAN */
static float calcolaSomma(float a, float b)
{
    char url[256];
    char response[MTU];

    /* build REST URL */
    snprintf(url, sizeof url,
             "http://localhost:8000/calcola-somma?param1=%f&param2=%f", a, b);

    int httpCode = doGET(url, response, sizeof response);
    if (httpCode != 200) {
        fprintf(stderr, "[CLIENT] HTTP error %d\n", httpCode);
        return 0;
    }

    /* response now contains only the body thanks to doGET() */
    float somma = 0.0f;
    if (sscanf(response, "{\"somma\":%f}", &somma) != 1) {
        fprintf(stderr, "[CLIENT] Impossibile parse JSON: %s\n", response);
        return 0;
    }
    return somma;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <numero1> <numero2>\n", argv[0]);
        return 1;
    }
    float v1 = atof(argv[1]);
    float v2 = atof(argv[2]);

    float risultato = calcolaSomma(v1, v2);
    if (risultato != 0.0f) {
        printf("%f + %f = %f\n", v1, v2, risultato);
    }
    return 0;
}
