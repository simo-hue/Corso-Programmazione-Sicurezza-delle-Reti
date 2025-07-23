#include "network.h"

/***************** BUSINESS LOGIC *****************/
/* Funzione utilitaria: prova di primalità */
static bool is_prime(int n){
    if(n < 2) return false;
    for(int j = 2; j*j <= n; ++j)
        if(n % j == 0) return false;
    return true;
}

/***************** HELPER HTTP *****************/
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

static void discard_body(FILE *stream, long length){
    for(long i = 0; i < length; ++i) fgetc(stream);
}

/***************** MAIN *****************/
int main(void){
    socketif_t sockfd = createTCPServer(8000);
    if(sockfd < 0){ fprintf(stderr,"[SERVER] Errore socket %d\n",sockfd); return 1; }
    printf("[SERVER] In ascolto porta 8000…\n");

    for(;;){
        FILE *connfd = acceptConnectionFD(sockfd);
        if(!connfd) continue;

        char line[MTU] = {0};
        char method[8] = {0};
        char url[MTU]  = {0};

        /* ---- request‑line ---- */
        if(!fgets(line,sizeof(line),connfd)){ fclose(connfd); continue; }
        sscanf(line, "%7s %511s", method, url);

        /* ---- header ---- */
        long cl = 0;
        while(fgets(line,sizeof(line),connfd)){
            if(strcmp(line,"\r\n")==0) break;
            if(strncmp(line,"Content-Length:",15)==0) cl = atol(line+15);
        }
        if(strcmp(method,"POST")==0||strcmp(method,"PUT")==0) discard_body(connfd,cl);

        /* ---- routing ---- */
        if(strstr(url,"calcola-somma")){
            float a=0,b=0;
            if(sscanf(url,"%*[^?]?param1=%f&param2=%f",&a,&b)!=2){
                send_json(connfd,400,"{ \"errore\": \"Parametri mancanti o malformati\" }\n");
            }else{
                char body[128];
                snprintf(body,sizeof(body),"{ \"somma\": %.2f }\n",a+b);
                send_json(connfd,200,body);
            }
        }
        else if(strstr(url,"numeri-primi")){
            int a=0,b=0;
            if(sscanf(url,"%*[^?]?param1=%d&param2=%d",&a,&b)!=2||a>b){
                send_json(connfd,400,"{ \"errore\": \"Intervallo non valido\" }\n");
            }else{
                /* costruiamo l'array JSON dei primi */
                char primiBuf[MTU]="";      // contenitore numeri separati da virgola
                int  counter=0;
                char tmp[16];
                for(int i=a;i<=b;++i){
                    if(is_prime(i)){
                        if(counter>0) strncat(primiBuf,",",sizeof(primiBuf)-strlen(primiBuf)-1);
                        snprintf(tmp,sizeof(tmp),"%d",i);
                        strncat(primiBuf,tmp,sizeof(primiBuf)-strlen(primiBuf)-1);
                        ++counter;
                    }
                }
                char body[MTU*2];
                snprintf(body,sizeof(body),
                         "{ \"totale_numeri_primi\": %d, \"primi\": [%s] }\n",
                         counter, primiBuf);
                send_json(connfd,200,body);
            }
        }
        else{
            send_json(connfd,404,"{ \"errore\": \"Funzione non riconosciuta\" }\n");
        }

        fclose(connfd);
        printf("[SERVER] Connessione terminata.\n\n");
    }

    closeConnection(sockfd);
    return 0;
}
