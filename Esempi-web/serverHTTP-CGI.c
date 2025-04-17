#include "network.h"

void sommatrice(char *url, FILE *out)  {
   char *function, *op1, *op2;
   float somma, val1, val2;
   
   function = strtok(url, "?&");
   op1 = strtok(NULL, "?&");
   op2 = strtok(NULL, "?&");
   strtok(op1,"=");
   val1 = atof(strtok(NULL,"="));
   strtok(op2,"=");
   val2 = atof(strtok(NULL,"="));
   
   somma = val1 + val2;
   
   fprintf(out,"HTTP/1.1 200 OK\r\n\r\n<html><head><title>Risultato</title></head><body>Risultato=%f</body></html>\r\n\r\n", somma);
}

int main(){
    socketif_t sockfd;
    FILE* connfd;
    int res, i;
    long length=0;
    char request[MTU], url[MTU], method[10], c;
    char *html="HTTP/1.1 200 OK\r\n\r\n<html><head><title>An Example Page</title></head><body>Hello World, this is a very simple HTML document.</body></html>\r\n\r\n";
    
    sockfd = createTCPServer(8000);
    if (sockfd < 0){
        printf("[SERVER] Errore: %i\n", sockfd);
        return -1;
    }
    
    while(true) {
        connfd = acceptConnectionFD(sockfd);
        
        fgets(request, sizeof(request), connfd);
        strcpy(method,strtok(request, " "));
        strcpy(url,strtok(NULL, " "));
        while(request[0]!='\r') {
            fgets(request, sizeof(request), connfd);
            if(strstr(request, "Content-Length:")!=NULL)  {
                length = atol(request+15);
            }
        }
        
        if(strcmp(method, "POST")==0)  {
            for(i=0; i<length; i++)  {
                c = fgetc(connfd);
            }
        }
        
        if(strstr(url, "sommatrice")==NULL)  {
            printf("Pagina statica\n");
            fputs(html, connfd);
        }
        else {
            printf("Pagina dinamica\n");
            // passo al programma CGI sia url sia stream su cui scrivere
            sommatrice(url, connfd);
        }
        
        fclose(connfd);
                
        printf("\n\n[SERVER] sessione HTTP completata\n\n");
    }
    
    closeConnection(sockfd);
    return 0;
}



