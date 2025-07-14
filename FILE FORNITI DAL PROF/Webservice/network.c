#include "network.h"

int inits = 0;

FILE *createTCPConnectionFD(char *host, int destPort){
    lib_init();
    struct sockaddr_in serv_addr;
    struct hostent *server;
    connection_t connID;
    FILE *fd;
    
    connID = socket(AF_INET, SOCK_STREAM, 0);
    if (connID < 0) exit(ERRSOCKET);
    
    server = gethostbyname(host);
    if (server == NULL) exit(ERRHOST);
    
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(destPort);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);
    
    if (connect(connID,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        exit(ERRCONNECT);
    
    fd = fdopen(connID, "w+");
    return fd;
}

connection_t createTCPConnection(char *host, int destPort){
    lib_init();
    struct sockaddr_in serv_addr;
    struct hostent *server;
    connection_t connID;
    
    connID = socket(AF_INET, SOCK_STREAM, 0);
    if (connID < 0) exit(ERRSOCKET);
    
    server = gethostbyname(host);
    if (server == NULL) exit(ERRHOST);
    
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(destPort);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);
    
    if (connect(connID,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        exit(ERRCONNECT);
    
    return connID;
}

socketif_t createTCPServer(int sourcePort){
    lib_init();
    struct sockaddr_in serv_addr;
    connection_t sockfd;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) exit(ERRSOCKET);
    
    memset(&serv_addr,'0',sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(sourcePort);
    
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes)) < 0){
        exit(ERRREUSEADDR);
    }
    
    if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        exit(ERRBIND);
    }
    
    listen(sockfd, 10);
    return sockfd;
}

socketif_t createUDPInterface(int sourcePort){
    lib_init();
    struct sockaddr_in serv_addr;
    socketif_t sockfd;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) exit(ERRSOCKET);
    
    if (sourcePort !=-1){
        memset(&serv_addr,0,sizeof(serv_addr));
    
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(sourcePort);
    
        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes)) < 0){
            exit(ERRREUSEADDR);
        }
    
        if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
            exit(ERRBIND);
    }
    return sockfd;
}

int UDPReceive(socketif_t sockfd, void *message, int msgLen, char *host, int *sourcePort){
    int respLen;
    socklen_t len;
    struct sockaddr_in info;
    
    len = sizeof(info);
    
    respLen = recvfrom((int)sockfd, message, msgLen, 0, ( struct sockaddr *) &info, &len);
    //buffer[respLen] = '\0';
    
    strcpy(host, inet_ntoa(info.sin_addr));
    *sourcePort = ntohs(info.sin_port);
    return respLen;
}

int UDPSend(socketif_t sockfd, void *message, int msgLen, char *host, int destPort){
    struct sockaddr_in serv_addr;
    struct hostent *s;
    
    s = gethostbyname(host);
    if (s == NULL) exit(ERRHOST);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(destPort);
    memcpy(&serv_addr.sin_addr.s_addr,s->h_addr,s->h_length);
    
    int n = sendto((int)sockfd, message, msgLen, 0, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
    return 0;
}

int TCPSend(connection_t connID, void *message, int msgLen)
{
        int byteSent = send(connID, message, msgLen, 0);
        return byteSent;
}

int TCPReceive(connection_t connID, void *message, int msgLen)
{
        //struct timeval tv;
        //tv.tv_sec = 20;
        //tv.tv_usec = 0;
        //if(setsockopt(connID, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(tv)) < 0)
        //    exit(ERRTIME);
        
    
        int byteReceived = recv(connID, message, msgLen, 0);
        if (byteReceived > msgLen)
            exit(ERRLEN);
        return byteReceived;
}

FILE *acceptConnectionFD(socketif_t sockfd){
    FILE *fd;
    connection_t connID = accept(sockfd, (struct sockaddr*)NULL, NULL);
    if (connID < 0) exit(ERRACCEPT);
    
    fd = fdopen(connID, "w+");
    return fd;
}

connection_t acceptConnection(socketif_t sockfd){
    connection_t connID = accept(sockfd, (struct sockaddr*)NULL, NULL);
    if (connID < 0) exit(ERRACCEPT);
    return connID;
}

int closeConnection(connection_t connID){
    CLOSE_SOCKET_FUNCTION(connID);
    return SUCCESS;
}

int createHTTPRequest(int method, char *path, char *body, char *request, int len){

    if(method == GET){
        sprintf(request, "%s %s HTTP/1.1\r\n\r\n","GET",path);
        return strlen(request)<len?strlen(request):ERRLEN;
    }
    else if(method == POST){
        if (body!=NULL){
            sprintf(request, "%s %s HTTP/1.1\r\nContent-Type: plain/text\r\nContent-Length: %li\r\n\r\n", "POST",path,strlen(body));
            strcat(request, body);
            return strlen(request)<len?strlen(request):ERRLEN;
        }
        else
            exit(ERRLEN);
    }
    else if(method == PUT){
        if (body!=NULL){
            sprintf(request, "%s %s HTTP/1.1\r\nContent-Type: plain/text\r\nContent-Length: %li\r\n\r\n", "PUT",path,strlen(body));
            strcat(request, body);
            return strlen(request)<len?strlen(request):ERRLEN;
        }
        else
            exit(ERRLEN);
    }
    else if(method == DEL){
        sprintf(request, "%s %s HTTP/1.1\r\n\r\n", "DELETE", path);
        return strlen(request)<len?strlen(request):ERRLEN;
    }
    return -1;
}

int doHTTPRequest(char *host, int port, char *request, char *response, int respLen){
    lib_init();
    connection_t connID;
    int bytes, sent, received, total;
    
    connID = createTCPConnection(host,port);
    
    TCPSend(connID, request, strlen(request));
    
    received = TCPReceive(connID, response, MTU);
    if (received > respLen){
        exit(ERRLEN);
        closeConnection(connID);
    }
    
    closeConnection(connID);
    return SUCCESS;
}

int doGET(char *url, char *response, int respLen){
    char ip_addr[12], path[100], request[MTU];
    int port;
    int status = sscanf(url,"%*[^:]%*[:/]%[^:]:%d%s", ip_addr, &port, path);
    int n = createHTTPRequest(GET, path, NULL, request, MTU);
    n = doHTTPRequest(ip_addr, port, request, response, respLen);
    
    char code[3];
    code[0] = response[9];
    code[1] = response[10];
    code[2] = response[11];  
    status = atoi(code);
    
    char *data = strstr(response, "\r\n\r\n" );
    if ( data == NULL )
        exit(ERRNOBODY);    
    data += 4;
    strcpy(response, data);
    return status;
}

int doPOST(char *url, char *body, char *response, int respLen){
    char ip_addr[12], path[100], request[MTU];
    int port;
    int status = sscanf(url,"%*[^:]%*[:/]%[^:]:%d%s", ip_addr, &port, path);
    int n = createHTTPRequest(POST, path, body, request, MTU);
    n = doHTTPRequest(ip_addr, port, request, response, respLen);
    
    char code[3];
    code[0] = response[9];
    code[1] = response[10];
    code[2] = response[11];  
    status = atoi(code);
    
    char *data = strstr(response, "\r\n\r\n" );
    if ( data == NULL )
        exit(ERRNOBODY);    
        data += 4;
        strcpy(response, data);
        return status;
}

int doPUT(char *url, char *body, char *response, int respLen){
    char ip_addr[12], path[100], request[MTU];
    int port;
    int status = sscanf(url,"%*[^:]%*[:/]%[^:]:%d%s", ip_addr, &port, path); //parserizza l' url
    int n = createHTTPRequest(PUT, path, body, request, MTU);   //crea e mette in request la richiesta HTTP
    n = doHTTPRequest(ip_addr, port, request, response, respLen);   //effettua la richiesta HTTP tramite i socket verso i parametri specificati
    
    char code[3];
    code[0] = response[9];
    code[1] = response[10];
    code[2] = response[11];  
    status = atoi(code);
    
    char *data = strstr(response, "\r\n\r\n" );
    if ( data == NULL )
        exit(ERRNOBODY);    
        data += 4;
        strcpy(response, data);
        return status;
}

int doDELETE(char *url, char *response, int respLen){
    char ip_addr[12], path[100], request[MTU];
    int port;
    int status = sscanf(url,"%*[^:]%*[:/]%[^:]:%d%s", ip_addr, &port, path); //parserizza l' url
    int n = createHTTPRequest(DEL, path, NULL, request, MTU);   //crea e mette in request la richiesta HTTP
    n = doHTTPRequest(ip_addr, port, request, response, respLen);   //effettua la richiesta HTTP tramite i socket verso i parametri specificati
    
    char code[3];
    code[0] = response[9];
    code[1] = response[10];
    code[2] = response[11];  
    status = atoi(code);
    
    char *data = strstr(response, "\r\n\r\n" );
    if ( data == NULL )
        exit(ERRNOBODY);    
        data += 4;
        strcpy(response, data);
        return status;
}

int createConcurrentNetworkFunction(void *networkID, void *function(void *)){
    pthread_t thread;
    int retval = 0;
    retval = pthread_create(&thread, NULL, function, (void*)networkID);
    return retval;
}

int lib_init(){
        if (inits == 0){
            INIT_SOCKET_SYSTEM //for windows
            inits = 1;
        }
        return 0;
}
