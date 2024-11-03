/*
server.c
- This program creates a concurrent TCP server that listens on IP 0.0.0.0 and port 10200.
- For each connected client, it forks a new process.
- The server sends the current date, time, and process ID (PID) to each client.
*/

#include <stdio.h>      // Standard I/O library
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String manipulation functions
#include <unistd.h>     // POSIX API for UNIX system calls
#include <sys/socket.h> // Socket API
#include <sys/types.h>  // Data types used in system calls
#include <netinet/in.h> // Structures for internet addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <time.h>       // Time manipulation functions

#define PORTNO 10200    // Port number for server connection

int main() {
    int sockfd, newsockfd;                // Socket descriptors
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[256];                     // Buffer to store date, time, and PID
    time_t rawtime;
    struct tm *timeinfo;

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address parameters
    server_addr.sin_family = AF_INET;          // Address family (IPv4)
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP
    server_addr.sin_port = htons(PORTNO);      // Port number

    // Bind the socket to the IP and port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Daytime server is running...\n");

    // Infinite loop to accept and handle multiple clients concurrently
    while (1) {
        client_len = sizeof(client_addr);

        // Accept an incoming client connection
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (newsockfd < 0) {
            perror("Accept failed");
            continue;
        }

        // Fork a new process to handle the client
        if (fork() == 0) {
            // Child process: handle the client
            close(sockfd); // Child does not need the listening socket

            // Get the current date and time
            time(&rawtime);
            timeinfo = localtime(&rawtime);

            // Format the date, time, and PID into the buffer
            snprintf(buffer, sizeof(buffer), "Date and Time: %sProcess ID: %d\n", asctime(timeinfo), getpid());

            // Send the date, time, and PID to the client
            write(newsockfd, buffer, strlen(buffer));

            close(newsockfd); // Close the client socket after sending the message
            exit(0);          // Exit the child process
        } else {
            // Parent process: close the client socket and continue accepting new clients
            close(newsockfd);
        }
    }

    close(sockfd); // Close the server socket (not reached in this example)
    return 0;
}
/*
client.c
- This program creates a TCP client that connects to the daytime server on IP 127.0.0.1 and port 10200.
- The client receives the current date, time, and server process ID (PID) from the server and displays it.
*/

#include <stdio.h>      // Standard I/O library
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String manipulation functions
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for internet addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls

#define PORTNO 10200    // Port number for server connection

int main() {
    int sockfd;                             // Socket descriptor
    struct sockaddr_in server_addr;         // Structure for server address
    char buffer[256];                       // Buffer to store the server response

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address parameters
    server_addr.sin_family = AF_INET;                     // Address family (IPv4)
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP (localhost)
    server_addr.sin_port = htons(PORTNO);                 // Port number

    // Connect to the daytime server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Receive the date, time, and PID from the server
    int bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Read failed");
    } else {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer
        printf("Received from server:\n%s", buffer); // Display the server response
    }

    close(sockfd); // Close the socket after receiving the message
    return 0;
}
