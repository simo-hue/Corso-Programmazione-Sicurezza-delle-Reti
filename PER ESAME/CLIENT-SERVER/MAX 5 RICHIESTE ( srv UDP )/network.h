/** 
* @file canpie.h
* @author Lorenzo Castagna
* @date 1 Gen 2022
* @brief C-based Abstracted Network Programming Interface for Education
*
* This library allows you to perform network operations in a simple way avoiding all the implementation details.
* You can send UDP/TCP packets to a server or receive them.
* You can send HTTP request to a server and get the response.
* All of this is achieved by the use of the sockets
*
*/

#ifdef __WIN32__
#include <winsock2.h> // used for sockets with windows, needs startup / shutdown
#include <ws2tcpip.h> // for MinGW / socklen_t
#define INIT_SOCKET_SYSTEM WSADATA wsaData;if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {printf ("Error initialising WSA.\n");exit(6);}
#define CLEAR_SOCKET_SYSTEM WSACleanup();
#define CLOSE_SOCKET_FUNCTION closesocket
#else
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define INIT_SOCKET_SYSTEM return 0;
#define CLEAR_SOCKET_SYSTEM return 0;
#define CLOSE_SOCKET_FUNCTION close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

#define ERRSOCKET -1        ///< Error creating the socket
#define ERRHOST -2          ///< Error, no such host
#define ERRCONNECT -3       ///< Error, failed connection
#define ERRBIND -4          ///< Error during socket binding
#define ERRACCEPT -5        ///< Error during connection acceptance
#define ERRLEN -6           ///< Error, the size of the string is insufficent
#define ERRSEND -7          ///< Error sending message
#define ERREAD -8           ///< Error reading
#define ERRFULLRECEIVED -9  ///< Error, the message is not fully received
#define ERRNOBODY -10       ///< Error, no body specified
#define ERRREUSEADDR -11    ///< Error, cant force socket to use an already binded port
#define ERRTIME -12         ///< Error, time out operation

#define SUCCESS 0           ///< Constant returned when an operation is successful

#define GET 1               ///< Constant for GET method
#define POST 2              ///< Constant for POST method
#define PUT 3               ///< Constant for PUT method
#define DEL 4               ///< Constant for DELETE method

#define MTU 500             ///< Default length in byte for message strings
#define MAXADDRESSLEN 50    ///< Default length in byte for address strings

#define AUTO -1             ///< To avoid the explicit UDP port indication in case of client 

/**
* Macros to simplify the implementation of thread functions
**/
#define EXTRACT_SOCKET(X)  (*(socketif_t *)(X))
#define EXTRACT_CONNECTION(X)  (*(connection_t *)(X))
#define EXTRACT_FD(X)  (*(FILE *)(X))

/**
* Type definition for the socket interface
**/
typedef int socketif_t;

/**
* Type definition for the TCP connection identifier
**/
typedef int connection_t;

/**
* @brief Function used to create a UDP interface and bind it to a specified local port, 
* if AUTO is specified a random port will be assigned for binding
*
* @param[in] sourcePort The local port
*
* @retval ID of the created socked
* @retval ERRSOCKET Error during creation of the socket
* @retval ERRBIND Error during the bind of socket to the specified port
*
*/
socketif_t createUDPInterface(int sourcePort);

/**
* @brief Function used to wait for a UDP packet from an unidentified source
*
* @param[in] sockfd ID of the socket used to receive the message
* @param[in] message Buffer where the message will be stored
* @param[in] msgLen Buffer length
* @param[in] host String where the host address of the source will be stored
* @param[in] sourcePort Pointer to the integer where the port number of the source will be stored
*
* @retval Length in byte of the received message
*
*/
int UDPReceive(socketif_t sockfd, void *message, int msgLen, char *host, int *sourcePort);

/**
* @brief Function used to send a UDP packet to a specified host and port
*
* @param[in] sockfd ID of the socket used to send the message
* @param[in] message Buffer containing the message to be sent 
* @param[in] msgLen Buffer length
* @param[in] host String containing destination IP/hostname
* @param[in] destPort Destination port
*
* @retval ERRHOST Error no host with this hostname
*
*/
int UDPSend(socketif_t sockfd, void *message, int msgLen, char *host, int destPort);

/**
* @brief Function used by the server to create a TCP interface and bind the server info (port, ip) 
*
* @param[in] sourcePort The local port of the server
*
* @retval ID of the created socket 
* @retval ERRSOCKET Error during creation of the socket
* @retval ERRBIND Error during the bind of socket to the specified port
*
*/
socketif_t createTCPServer(int sourcePort);

/**
* @brief Function used by the client to create a TCP connection towards a host and the specified port
*
* @param[in] host The host to connect to (IP or Hostname) 
* @param[in] destPort The destination port
*
* @retval ID of the established connection 
* @retval ERRSOCKET Error during creation of the socket
* @retval ERRHOST Error no host with this hostname
* @retval ERRCONNECT Error during connection to the server
*
*/
connection_t createTCPConnection(char *host, int destPort);

/**
* @brief Function used by the client to create a TCP connection towards a host and the specified port, 
* the connection ID is returned as a stream file descriptor
*
* @param[in] host The host to connect to (IP or Hostname) 
* @param[in] destPort The destination port
*
* @retval File descriptor of the established connection
* @retval ERRSOCKET Error during creation of the socket
* @retval ERRHOST Error no host with this hostname
* @retval ERRCONNECT Error during connection to the server
*
*/
FILE *createTCPConnectionFD(char *host, int destPort);

/**
* @brief Function used by the server to accept a connection coming from a client
*
* @param[in] sockfd ID of the socket where a connection will be accepted
*
* @retval ID of the accepted connection 
* @retval ERRACCEPT Error during acceptance of the connection
*
*/
connection_t acceptConnection(socketif_t sockfd);

/**
* @brief Function used by the server to wait for a connection coming from a client, 
* the connection ID is the form of a stream file descriptor
*
* @param[in] sockfd ID of the socket where a connection will be accepted
*
* @retval File descriptor of the accepted connection
* @retval ERRACCEPT Error during acceptance of the connection
*
*/
FILE *acceptConnectionFD(socketif_t sockfd);

/**
* @brief Function used to send a message through an established TCP connection
*
* @param[in] connID ID of the TCP connection
* @param[in] message String containing the message to be sent
* @param[in] msgLen Length in byte of the message 
*
* @retval byteSent Number of byte sent through the connection
*
*/
int TCPSend(connection_t connID, void *message, int msgLen);

/**
* @brief Function used to receive a message through an established TCP connection
* 
* @param[in] connID ID of the TCP connection
* @param[in] message String to store the received message 
* @param[in] msgLen Length in byte of the string to store the received message 
*
* @retval ERRLEN Error the length of the message received is bigger than expected
*
*/
int TCPReceive(connection_t connID, void *message, int msgLen);

/**
* @brief Close the specified connection
* 
* @param[in] connID ID of the established TCP connection
*
* @retval SUCCESS Successful closure of the connection
*
*/
int closeConnection(connection_t connID);

/**
* @brief Function that create the text for the HTTP request specified
* 
* @param[in] method Request method (GET,POST,PUT,DELETE)
* @param[in] path Path of the resource reaquired
* @param[in] body Body required (NULL if not needed in the request)
* @param[in] request String where the request will be stored
* @param[in] len Length of the request String
* @param[out] requestLen Length of the text request generated
*
* @retval ERRLEN Error the length of the request string is not enough 
*
*/
int createHTTPRequest(int method, char *path, char *body, char *request, int len);

/**
* @brief Function that do the specified request on the host/port specified
*
* @param[in] host Destination IP/hostname
* @param[in] port Destination Port (80 default for HTTP)
* @param[in] request String containing the HTTP request
* @param[in] response String where the response with the header will be stored
* @param[in] respLen Length of the response string
* @param[out] SUCCESS Operation is successful
*
* @retval ERRLEN Error the length of the response string is not enough
*
*/

int doHTTPRequest(char *host, int port, char *request, char *response, int respLen);

/**
* @brief Function used by the user to make a GET request
*
* @param[in] url Complete URL of the resource required
* @param[in] response String where the response without the header will be stored
* @param[in] respLen Length of the response string
* @param[out] status Return status of the HTTP Request
*
* @retval ERRNOBODY Error there is no body in the response
*
*/
int doGET(char *url, char *response, int respLen);

/**
* @brief Function used by the user to make a POST request
*
* @param[in] url Complete URL of the resource required
* @poram[in] body Body of the POST request
* @param[in] response String where the response without the header will be stored
* @param[in] respLen Length of the response string
* @param[out] status Return status of the HTTP Request
*
* @retval ERRNOBODY Error there is no body in the response
*/

int doPOST(char *url, char *body, char *response, int respLen);

/**
* @brief Function used by the user to make a PUT request
*
* @param[in] url Complete URL of the resource required
* @poram[in] body Body of the PUT request
* @param[in] response String where the response without the header will be stored
* @param[in] respLen Length of the response string
* @param[out] status Return status of the HTTP Request
*
* @retval ERRNOBODY Error there is no body in the response
*/
int doPUT(char *url, char *body, char *response, int respLen);

/**
* @brief Function used by the user to make a DELETE request
*
* @param[in] url Complete URL of the resource required
* @poram[in] body Body of the DELETE request
* @param[in] response String where the response without the header will be stored
* @param[in] respLen Length of the response string
* @param[out] status Return status of the HTTP Request
*
* @retval ERRNOBODY Error there is no body in the response
*/
int doDELETE(char *url, char *response, int respLen);

/**
* @brief Function to create a thread that runs the function received as input
*
* @param[in] networkID Network ID of the network resource (socket ID or connection ID or file descriptor) used inside the function
* @param[in] function Function pointer to the function (defined by the user) that the thread will execute
* 
* @retval Return value from pthread function
*/
int createConcurrentNetworkFunction(void *networkID, void *function(void *));

/**
* @brief Function used to inizialize the library on windows
*
*/
int lib_init();

