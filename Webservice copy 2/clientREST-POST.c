#include "network.h"

int main(){
    char response[MTU];
    char *body = "{\r\n  \"Id\": 12345,\r\n  \"Customer\": \"John Smith\",\r\n  \"Quantity\": 1,\r\n  \"Price\": 10.00\r\n}";

    int res = doPOST("http://localhost:80/prova", body, response, MTU);
    if (res < 0){
        printf("Errore: %i\n", res);
        return -1;
    }
    printf("Status code: %i\n", res);
    printf("Risposta dal server: %s\n", response);
    return 0;
}
