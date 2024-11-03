/*
Question:
Implement a simple TCP client-server program in C where:
- The server listens on a specified port, accepts connections, and echoes back messages received from the client.
- The client sends a message to the server, receives the echoed message, and displays it.
Note: Replace the IP address in both the server and client with your system's IP address.
*/

/*
server.c
- This program creates a TCP server that listens on IP 172.16.59.10 and port 10200.
- For each connected client, it forks a child process to handle communication.
- The server reads a message from the client and echoes it back.
*/

#include <stdio.h>      // Standard I/O library
#include <string.h>     // String manipulation functions
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for internet addresses
#include <unistd.h>     // POSIX API for UNIX system calls
#include <stdlib.h>     // Standard library functions

#define PORTNO 10200    // Port number for the server

int main() {
    int sockfd, newsockfd, clilen, n;
    char buf[256];                             // Buffer to store client message
    struct sockaddr_in seraddr, cliaddr;       // Structures for server and client addresses

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address parameters
    seraddr.sin_family = AF_INET;              // Address family (IPv4)
    seraddr.sin_addr.s_addr = inet_addr("172.16.59.10"); // Server IP (replace with your IP)
    seraddr.sin_port = htons(PORTNO);          // Port number in network byte order

    // Bind the socket to the specified IP and port
    if (bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections, with a maximum backlog of 5 connections
    listen(sockfd, 5);
    printf("Server is listening for incoming connections...\n");

    // Infinite loop to accept and handle clients
    while (1) {
        clilen = sizeof(cliaddr);

        // Accept an incoming client connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
        if (newsockfd < 0) {
            perror("Accept failed");
            continue;
        }

        // Fork a child process to handle the connected client
        if (fork() == 0) {
            // Child process: read from and write to the client
            n = read(newsockfd, buf, sizeof(buf));
            if (n < 0) {
                perror("Read failed");
                close(newsockfd);
                exit(EXIT_FAILURE);
            }
            printf("\nMessage from Client: %s\n", buf);

            // Echo back the received message
            n = write(newsockfd, buf, sizeof(buf));
            if (n < 0) {
                perror("Write failed");
                close(newsockfd);
                exit(EXIT_FAILURE);
            }

            close(newsockfd);  // Close the client socket
            exit(0);            // Exit the child process
        } else {
            // Parent process: close the socket for this client and continue to accept new clients
            close(newsockfd);
        }
    }

    close(sockfd); // Close the server socket (not reached in this code)
    return 0;      // Exit program
}
/*
client.c
- This program creates a TCP client that connects to a server on IP 172.16.56.10 and port 9704.
- It prompts the user to enter a message, sends this message to the server, and receives the echoed message back.
*/

#include <stdio.h>      // Standard I/O library
#include <stdlib.h>     // Standard library functions
#include <fcntl.h>      // File control options
#include <string.h>     // String manipulation functions
#include <sys/socket.h> // Socket API
#include <sys/types.h>  // Data types used in system calls
#include <netinet/in.h> // Structures for internet addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls

#define PORT 9704       // Port number for server connection

int main() {
    int sd;                                 // Socket descriptor
    struct sockaddr_in address;             // Structure for server address

    // Create a TCP socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address parameters
    address.sin_family = AF_INET;                           // Address family (IPv4)
    address.sin_addr.s_addr = inet_addr("172.16.56.10");    // Server IP (replace with your IP)
    address.sin_port = htons(PORT);                         // Port number in network byte order

    // Connect to the server
    if (connect(sd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Connection failed");
        close(sd);
        exit(EXIT_FAILURE);
    }

    // Prompt user for input
    char buf[25];
    printf("Enter a message: ");
    fgets(buf, sizeof(buf), stdin);                         // Read message from user

    // Send the message to the server
    int m = send(sd, buf, strlen(buf), 0);
    if (m < 0) {
        perror("Send failed");
        close(sd);
        exit(EXIT_FAILURE);
    }

    // Receive the echoed message from the server
    int n = recv(sd, buf, sizeof(buf), 0);
    if (n < 0) {
        perror("Receive failed");
        close(sd);
        exit(EXIT_FAILURE);
    }
    
    printf("The server echoed: %s\n", buf);                 // Display echoed message

    close(sd);                                              // Close the socket
    return 0;                                               // Exit program
}
