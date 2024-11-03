/*
client.c
- This program creates a TCP client that connects to a chat server on IP 0.0.0.0 and port 10200.
- The client has two threads: one for reading messages from the server and one for sending messages to the server.
*/

#include <stdio.h>      // Standard I/O library
#include <stdlib.h>     // Standard library functions
#include <fcntl.h>      // File control options
#include <sys/socket.h> // Socket API
#include <sys/types.h>  // Data types used in system calls
#include <netinet/in.h> // Structures for internet addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls
#include <string.h>     // String manipulation functions
#include <pthread.h>    // POSIX threads library

#define PORTNO 10200    // Port number for server connection

int sd;                 // Socket descriptor

// Thread function to read messages from the server
void* read_messages(void* arg) {
    char output[300];
    while (1) {
        int bytes_read = read(sd, output, sizeof(output) - 1);
        if (bytes_read > 0) {
            output[bytes_read] = '\0'; // Null-terminate the string
            printf("Received: %s\n", output);
        }
    }
    return NULL;
}

// Thread function to send messages to the server
void* write_messages(void* arg) {
    char buf[256];
    while (1) {
        fgets(buf, sizeof(buf), stdin);   // Get input from user
        write(sd, buf, strlen(buf));      // Send message to the server
    }
    return NULL;
}

int main() {
    struct sockaddr_in address;           // Structure for server address
    
    // Create a TCP socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    address.sin_family = AF_INET;                     // Address family (IPv4)
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP (localhost)
    address.sin_port = htons(PORTNO);                 // Port number
    
    // Connect to the server
    if (connect(sd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Connection failed");
        exit(1);
    }
    
    pthread_t read_thread, write_thread;
    
    // Create threads for reading and writing
    pthread_create(&read_thread, NULL, read_messages, NULL);
    pthread_create(&write_thread, NULL, write_messages, NULL);
    
    // Wait for both threads to finish (they won't in this case)
    pthread_join(read_thread, NULL);
    pthread_join(write_thread, NULL);
    
    close(sd);  // Close the socket when done
    return 0;
}
/*
server.c
- This program creates a TCP server that listens on IP 0.0.0.0 and port 10200.
- It accepts connections from up to three clients and handles each client in a separate thread.
- Messages received from any client are broadcasted to all connected clients.
*/

#include <stdio.h>      // Standard I/O library
#include <string.h>     // String manipulation functions
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for internet addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API for UNIX system calls
#include <pthread.h>    // POSIX threads library

#define PORTNO 10200    // Port number for server connection
#define MAX_CLIENTS 3   // Maximum number of clients

int clients[MAX_CLIENTS];      // Array to store client socket descriptors
int client_count = 0;          // Counter for connected clients

// Thread function to handle each client's messages
void* handle_client(void* arg) {
    int newsockfd = *(int*)arg;            // Client socket descriptor
    free(arg);                             // Free the allocated memory for socket descriptor
    char buf[256];                         // Buffer for messages
    
    while (1) {
        int h = read(newsockfd, buf, sizeof(buf) - 1); // Read message from client
        if (h <= 0)
            break;
        buf[h] = '\0';                     // Null-terminate the message
        
        // Format and display the received message
        char output[300];
        sprintf(output, "sent from %d: %s", newsockfd, buf);
        printf("%s\n", output);

        // Broadcast the message to all other clients
        for (int l = 0; l < client_count; l++) {
            if (clients[l] != newsockfd) { // Don't send to the sender itself
                write(clients[l], output, strlen(output));
            }
        }
    }
    return NULL;
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in seraddr, cliaddr;
    
    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    // Define server address parameters
    seraddr.sin_family = AF_INET;                  // Address family (IPv4)
    seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP (localhost)
    seraddr.sin_port = htons(PORTNO);              // Port number
    
    // Bind the socket to the IP and port
    if (bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }
    
    // Listen for incoming connections
    if (listen(sockfd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(1);
    }

    printf("Server is waiting for connections...\n");

    // Loop to accept and handle multiple client connections
    while (1) {
        socklen_t clilen = sizeof(cliaddr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
        if (newsockfd < 0) {
            perror("Accept failed");
            continue;
        }
        
        // If max clients not reached, add the new client
        if (client_count < MAX_CLIENTS) {
            clients[client_count++] = newsockfd;
            int* sockfd_ptr = malloc(sizeof(int)); // Allocate memory for client socket descriptor
            *sockfd_ptr = newsockfd;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handle_client, sockfd_ptr);
            pthread_detach(thread_id); // Detach thread to handle client independently
        } else {
            printf("Max clients reached. Connection refused.\n");
            close(newsockfd); // Close socket for clients beyond the limit
        }
    }
    
    close(sockfd);  // Close the server socket
    return 0;
}
