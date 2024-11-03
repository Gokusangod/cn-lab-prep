/*
Question:
Implement a simple client-server communication program using TCP in C.
- The client should send a string to the server.
- The server will receive this string and echo it back to the client.
- The communication should terminate after one exchange, and the server should handle multiple clients sequentially.
- The client will print the server’s response.
*/

/* 
client.c
- This program creates a TCP client that connects to the server on localhost and port 10200.
- It prompts the user to enter a string, sends this string to the server, and receives a response.
- The client displays the server's response and then closes the connection.
*/

// client.c code
#include <stdio.h>      // Standard input-output library
#include <string.h>     // String manipulation library
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for storing addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API for UNIX system calls
#define PORTNO 10200    // Port number for server connection

int main() {
    int sockfd, n;                          // sockfd: Socket file descriptor, n: for reading length
    struct sockaddr_in server_addr;         // Structure to store server address
    char buf[256];                          // Buffer for server response
    char ch[256];                           // Buffer for client message

    // Creating a socket (IPv4, TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");   // Error handling for socket creation
        exit(EXIT_FAILURE);                 // Exit if socket creation fails
    }

    // Defining server address parameters
    server_addr.sin_family = AF_INET;       // Address family (IPv4)
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost IP
    server_addr.sin_port = htons(PORTNO);   // Port number in network byte order

    // Connecting to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");        // Error handling for connection
        close(sockfd);                      // Close socket on failure
        exit(EXIT_FAILURE);
    }

    // Prompting user to enter a string to send to the server
    printf("Enter string to send: ");
    fgets(ch, sizeof(ch), stdin);           // Read input from user
    ch[strcspn(ch, "\n")] = '\0';           // Remove newline character

    // Sending message to the server
    if (write(sockfd, ch, strlen(ch)) < 0) {
        perror("write failed");             // Error handling for write
        close(sockfd);                      // Close socket on failure
        exit(EXIT_FAILURE);
    }

    // Reading server response
    memset(buf, 0, sizeof(buf));            // Clear buffer
    n = read(sockfd, buf, sizeof(buf) - 1); // Read response from server
    if (n < 0) {
        perror("read failed");              // Error handling for read
        close(sockfd);                      // Close socket on failure
        exit(EXIT_FAILURE);
    }
    buf[n] = '\0';                          // Null-terminate the response
    printf("Response from server: %s\n", buf); // Display server response

    close(sockfd);                          // Close the socket after communication ends
    return 0;                               // Exit program
}

/* 
server.c
- This program creates a TCP server that listens on port 10200 for incoming client connections.
- For each client, it reads the message sent by the client, prints it, and sends it back.
- The server can handle multiple clients sequentially.
- The connection is closed after each client exchange, and the server waits for the next client.
*/

// server.c code
#include <stdio.h>      // Standard input-output library
#include <string.h>     // String manipulation library
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for storing addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API for UNIX system calls
#define PORTNO 10200    // Port number for server connection

int main() {
    int sockfd, newsockfd, clilen, n;       // Socket descriptors and reading length
    struct sockaddr_in seraddr, cliaddr;    // Structures for server and client addresses
    char buf[256];                          // Buffer to store client message
    socklen_t addrlen;                      // Length of client address structure

    // Creating a socket (IPv4, TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");   // Error handling for socket creation
        exit(EXIT_FAILURE);                 // Exit if socket creation fails
    }

    // Defining server address parameters
    seraddr.sin_family = AF_INET;           // Address family (IPv4)
    seraddr.sin_addr.s_addr = INADDR_ANY;   // Accept connections from any IP address
    seraddr.sin_port = htons(PORTNO);       // Port number in network byte order

    // Binding the socket to the specified IP and port
    if (bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0) {
        perror("bind failed");              // Error handling for bind
        close(sockfd);                      // Close socket on failure
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections, with a maximum backlog of 5 connections
    if (listen(sockfd, 5) < 0) {
        perror("listen failed");            // Error handling for listen
        close(sockfd);                      // Close socket on failure
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for connections...\n");

    // Infinite loop to handle multiple clients
    while (1) {
        addrlen = sizeof(cliaddr);          // Set length of client address structure

        // Accepting a client connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &addrlen);
        if (newsockfd < 0) {
            perror("accept failed");        // Error handling for accept
            continue;                       // Continue loop to accept other clients
        }

        // Reading message from client
        memset(buf, 0, sizeof(buf));        // Clear buffer
        n = read(newsockfd, buf, sizeof(buf) - 1);
        if (n < 0) {
            perror("read failed");          // Error handling for read
            close(newsockfd);               // Close socket on failure
            continue;
        }

        buf[n] = '\0';                      // Null-terminate the client message
        printf("Message from Client: %s\n", buf); // Display client message

        // Echoing message back to the client
        n = write(newsockfd, buf, strlen(buf));
        if (n < 0) {
            perror("write failed");         // Error handling for write
        }
        close(newsockfd);                   // Close client socket after response
    }
    close(sockfd);                          // Close server socket (never reached in infinite loop)
    return 0;                               // Exit program
}
