/* =============================================================================
 * REST Web‑service example (sum of two floats)
 * -----------------------------------------------------------------------------
 * This *single file* contains **two independent C programs**:
 *   1. `server_http_rest.c`  —  minimal HTTP server exposing GET /calcola-somma
 *   2. `client_rest_get.c`  —  client stub that invokes the service via GET
 *
 * Save the two code blocks into **two separate files** and compile each of them
 * together with your fixed `mynetwork.c` (or `network.c`) library, e.g.:
 *     gcc server_http_rest.c mynetwork.c -o srv  -lpthread
 *     gcc client_rest_get.c  mynetwork.c -o cget -lpthread
 * -----------------------------------------------------------------------------
 * Both programs are heavily commented so that you can follow the network flow
 * step‑by‑step.
 * =============================================================================
 */

/************************ 1.  server_http_rest.c ******************************/

/*
 * Minimal REST server that accepts **one** endpoint:
 *     GET /calcola-somma?param1=<float>&param2=<float>
 * It returns JSON: { "somma": <result> }
 *
 * Dependencies:
 *   – your socket helper library  (network.h / mynetwork.c)
 *   – <ctype.h>, <string.h>, <stdio.h>
 */

#include "network.h"      /* helper library with sockets wrappers            */
#include <ctype.h>         /* for isdigit()                                   */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- business‑logic function ------------------------------------------------*/
static float calcolaSomma(float a, float b) { return a + b; }

/* ---- tiny percent‑decoder ---------------------------------------------------*/
/*  Decodes %xx in place.  (Only used for '\n' or space just in case) */
static void urlDecode(char *s)
{
    char *w = s;
    for (; *s; ++s, ++w) {
        if (*s == '%' && isxdigit((unsigned char)s[1]) && isxdigit((unsigned char)s[2])) {
            char hex[3] = { s[1], s[2], 0 };
            *w = (char)strtol(hex, NULL, 16);
            s += 2;
        } else {
            *w = *s;
        }
    }
    *w = '\0';
}

/* ---- main ------------------------------------------------------------------*/
int main(void)
{
    const int PORT = 8000;
    socketif_t server = createTCPServer(PORT);
    if (server < 0) {
        perror("createTCPServer");
        return 1;
    }
    printf("[SERVER] Listening on port %d...\n", PORT);

    /* infinite accept loop */
    while (1) {
        FILE *fd = acceptConnectionFD(server);      /* returns FILE* for stream I/O */
        if (!fd) { perror("acceptConnectionFD"); continue; }

        /* --- 1) read the request line (method path version) ----------------*/
        char line[MTU];
        if (!fgets(line, sizeof line, fd)) { fclose(fd); continue; }

        char method[8] = {0}, url[MTU] = {0};
        sscanf(line, "%7s %1023s", method, url);

        /* consume and ignore the rest of the headers -----------------------*/
        while (fgets(line, sizeof line, fd) && strcmp(line, "\r\n") != 0) {
            /* no body expected for GET, just skip */
        }

        /* --- 2) only accept GET /calcola-somma?... -------------------------*/
        int status = 200;
        char body[128] = {0};

        if (strcmp(method, "GET") != 0 || strncmp(url, "/calcola-somma", 14) != 0) {
            status = 404;
            strcpy(body, "{\"errore\":\"risorsa non trovata\"}");
        } else {
            /* parse query string */
            char *qs = strchr(url, '?');
            float p1 = 0, p2 = 0;
            if (qs) {
                ++qs;               /* skip '?' */
                urlDecode(qs);
                /* naive parsing; robust enough for the lab */
                sscanf(qs, "param1=%f&param2=%f", &p1, &p2);
                float somma = calcolaSomma(p1, p2);
                snprintf(body, sizeof body, "{\"somma\":%.6f}", somma);
            } else {
                status = 400;
                strcpy(body, "{\"errore\":\"parametri assenti\"}");
            }
        }

        /* --- 3) send HTTP response ----------------------------------------*/
        const char *msg = (status == 200) ? "OK" : (status == 404 ? "Not Found" : "Bad Request");
        char header[256];
        int lenBody = (int)strlen(body);
        int nHeader = snprintf(header, sizeof header,
                              "HTTP/1.1 %d %s\r\n"              /* status line        */
                              "Content-Type: application/json\r\n"
                              "Content-Length: %d\r\n"
                              "Connection: close\r\n"
                              "\r\n",                                   /* header/body separator */
                              status, msg, lenBody);
        fwrite(header, 1, nHeader, fd);
        fwrite(body,   1, lenBody, fd);
        fflush(fd);

        /* --- 4) close connection -----------------------------------------*/
        fclose(fd);
        printf("[SERVER] Handled one request (%d)\n", status);
    }
    closeConnection(server);
    return 0;
}