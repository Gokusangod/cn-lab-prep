/*
Question:
Create a TCP client-server program in C where:
- The client sends a string to the server.
- The server removes duplicate characters from the string and sends back the modified string.
- The client displays the modified string.
*/

/*
client.c
- This program creates a TCP client that connects to a server on IP 127.0.0.1 and port 10202.
- The client takes a string input from the user, sends it to the server, and receives the modified string with duplicate characters removed.
*/

#include <stdio.h>      // Standard I/O library
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String manipulation functions
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for internet addresses
#include <unistd.h>     // POSIX API for UNIX system calls
#include <arpa/inet.h>  // Definitions for internet operations

#define PORTNO 10202    // Port number for server connection

int sock, addrlen, client_fd, valread;
struct sockaddr_in address;             // Structure for server address
char str[100];                          // Buffer for input string
char result[100];                       // Buffer for result received from server

// Function to create and configure the client socket
void CreateClientSocket() {
    sock = socket(AF_INET, SOCK_STREAM, 0);           // Create a TCP socket
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;                     // Address family (IPv4)
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address (localhost)
    address.sin_port = htons(PORTNO);                 // Port number
    addrlen = sizeof(address);                        // Address length
}

// Function to connect to the server and send data
void PerformClientTask() {
    client_fd = connect(sock, (struct sockaddr *)&address, addrlen);
    if (client_fd == -1) {
        perror("\nCLIENT ERROR");
        exit(1);
    }

    // Input string from user
    printf("Enter string: ");
    scanf("%s", str);

    // Send the string to the server
    send(sock, str, strlen(str), 0);
    printf("String sent to server.\n");

    // Receive the modified string from the server
    memset(result, 0, sizeof(result));
    valread = read(sock, result, sizeof(result) - 1);
    if (valread < 0) {
        perror("Read failed");
        close(sock);
        exit(1);
    }
    result[valread] = '\0';

    // Display the result received from the server
    printf("Result from server: %s\n", result);
}

int main() {
    CreateClientSocket();  // Create and configure client socket
    PerformClientTask();   // Perform client task (send data and receive result)
    close(sock);           // Close the socket
    return 0;              // Exit program
}
/*
server.c
- This program creates a TCP server that listens on IP 127.0.0.1 and port 10202.
- For each connected client, it forks a child process to handle the client's string processing request.
- The server receives a string from the client, removes duplicate characters, and sends back the modified string.
*/

#include <stdio.h>      // Standard I/O library
#include <string.h>     // String manipulation functions
#include <stdlib.h>     // Standard library functions
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for internet addresses
#include <unistd.h>     // POSIX API for UNIX system calls
#include <arpa/inet.h>  // Definitions for internet operations

#define PORTNO 10202    // Port number for server connection

int server_fd, new_socket, addrlen, valread;
struct sockaddr_in address;             // Structure for server address
char str[100];                          // Buffer for string received from client
char result[100];                       // Buffer for modified string to send back to client
char msg[100];                          // Message buffer for sending result

// Function to create and configure the server socket
void CreateServerSocket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);      // Create a TCP socket
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;                     // Address family (IPv4)
    address.sin_addr.s_addr = INADDR_ANY;             // Accept connections from any IP
    address.sin_port = htons(PORTNO);                 // Port number
    addrlen = sizeof(address);                        // Address length
}

// Function to handle client requests for string processing
void PerformServerTask() {
    bind(server_fd, (struct sockaddr *)&address, addrlen); // Bind socket to IP and port
    printf("Server Waiting....\n");

    listen(server_fd, 5);  // Listen for incoming connections with a backlog of 5

    // Infinite loop to handle multiple client connections
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }

        // Fork a child process to handle the client's request
        if (fork() == 0) {
            close(server_fd); // Child does not need the listening socket

            // Clear buffers and read string from client
            memset(str, 0, sizeof(str));
            valread = read(new_socket, str, sizeof(str) - 1);
            if (valread < 0) {
                perror("Read failed");
                close(new_socket);
                exit(1);
            }
            str[valread] = '\0';

            // Remove duplicate characters from the string
            int length = strlen(str);
            for (int i = 0; i < length; i++) {
                if (str[i] != '0') {
                    for (int j = i + 1; j < length; j++) {
                        if (str[i] == str[j]) {
                            str[j] = '0'; // Mark duplicate characters as '0'
                        }
                    }
                }
            }

            // Copy unique characters to the result buffer
            int s = 0;
            for (int k = 0; k < length; k++) {
                if (str[k] != '0') {
                    result[s++] = str[k];
                }
            }
            result[s] = '\0'; // Null-terminate the result string

            // Send the modified string back to the client
            send(new_socket, result, strlen(result), 0);

            close(new_socket); // Close the client socket
            exit(0);           // Exit the child process
        } else {
            close(new_socket); // Parent process: close the client socket and continue to accept new clients
        }
    }
}

int main() {
    CreateServerSocket();        // Create and configure server socket
    PerformServerTask();         // Perform server task (handle client requests)
    shutdown(server_fd, SHUT_RDWR);  // Shutdown the server
    return 0;                    // Exit program
}
