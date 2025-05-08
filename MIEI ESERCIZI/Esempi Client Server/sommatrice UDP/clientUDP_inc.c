#include "network.h"

int main(void) {
    int request = -1;
    int response;
    socketif_t socket;
    char hostAddress[MAXADDRESSLEN];
    int port;
     
    // Inizializzazione dell’interfaccia socket sulla porta 20000
    socket = createUDPInterface(20000);
    
    while(request != 0){

        // Chiedo l'input
        printf("Inserisci un numero intero:\n");
        scanf("%d", &request);

        // Invia request tramite UDP al destinatario avente indirizzo IP "127.0.0.1" e porta 35000
        UDPSend(socket, &request, sizeof(request), "127.0.0.1", 35000);
         
        // Mettiti in attesa dell’arrivo di un messaggio UDP e metti il valore ricevuto in response
        UDPReceive(socket, &response, sizeof(response), hostAddress, &port);
         
        if(request != 0){
            printf("[CLIENT] Ho ricevuto un messaggio da host/porta %s/%d\n", hostAddress, port);
            printf("[CLIENT] Somma Parziale: %d\n", response);   
        }
    }
    printf("[CLIENT] Ho ricevuto un messaggio da host/porta %s/%d\n", hostAddress, port);
    printf("[CLIENT] Somma Finale: %d\n", response);
}

