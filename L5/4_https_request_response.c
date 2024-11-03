/*
client.c
- This program creates a TCP client that connects to a web server on a specified IP and port (usually 80 for HTTP).
- The client sends a manually crafted HTTP GET request and receives the HTTP response, displaying all headers and content.
*/

#include <stdio.h>      // Standard I/O library
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String manipulation functions
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for internet addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls

#define PORT 80         // Port number for HTTP requests (default for HTTP)
#define BUFFER_SIZE 4096 // Size of buffer to store the response

int main() {
    int sockfd;                          // Socket descriptor
    struct sockaddr_in server_addr;      // Structure for server address
    char request[512];                   // Buffer for HTTP request
    char response[BUFFER_SIZE];          // Buffer for HTTP response
    char *server_ip = "93.184.216.34";   // Example server IP (example.com); replace with actual IP if needed

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Define server address parameters
    server_addr.sin_family = AF_INET;                // Address family (IPv4)
    server_addr.sin_port = htons(PORT);              // Port number (HTTP)
    server_addr.sin_addr.s_addr = inet_addr(server_ip); // Server IP address (replace with actual IP if needed)

    // Connect to the web server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(1);
    }

    // Create a manually crafted HTTP GET request for the root page
    snprintf(request, sizeof(request), 
             "GET / HTTP/1.1\r\n"                  // HTTP method and path
             "Host: example.com\r\n"               // Host header (replace with actual domain if needed)
             "Connection: close\r\n"               // Connection header to close connection after response
             "\r\n");

    // Send the HTTP request to the server
    send(sockfd, request, strlen(request), 0);
    printf("HTTP request sent:\n%s\n", request);

    // Receive the HTTP response from the server and display all fields
    int bytes_received;
    printf("HTTP response received:\n");
    while ((bytes_received = read(sockfd, response, sizeof(response) - 1)) > 0) {
        response[bytes_received] = '\0';           // Null-terminate the response
        printf("%s", response);                    // Display the response
    }

    if (bytes_received < 0) {
        perror("Read failed");
    }

    close(sockfd);                                 // Close the socket
    return 0;
}
