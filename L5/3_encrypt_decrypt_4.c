/*
client.c
- This program creates a TCP client that connects to a server on IP 127.0.0.1 and port 10200.
- The client takes a message input from the user, encrypts it by adding 4 to the ASCII value of each character, and sends it to the server.
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
#define OFFSET 4        // Offset value for encryption

int main() {
    int sock;                               // Socket descriptor
    struct sockaddr_in address;             // Structure for server address
    char message[256];                      // Buffer for input message
    char encrypted_message[256];            // Buffer for encrypted message

    // Create a TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Define server address parameters
    address.sin_family = AF_INET;                     // Address family (IPv4)
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP (localhost)
    address.sin_port = htons(PORTNO);                 // Port number

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(1);
    }

    // Get input message from user
    printf("Enter a message to encrypt: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0'; // Remove newline character

    // Encrypt the message by adding OFFSET to each character
    int len = strlen(message);
    for (int i = 0; i < len; i++) {
        encrypted_message[i] = message[i] + OFFSET;
    }
    encrypted_message[len] = '\0'; // Null-terminate the encrypted message

    // Send the encrypted message to the server
    send(sock, encrypted_message, strlen(encrypted_message), 0);
    printf("Encrypted message sent: %s\n", encrypted_message);

    close(sock); // Close the socket after sending the message
    return 0;
}
/*
server.c
- This program creates a TCP server that listens on IP 127.0.0.1 and port 10200.
- The server receives an encrypted message from the client, decrypts it by subtracting 4 from each character, and displays both the encrypted and decrypted messages.
*/

#include <stdio.h>      // Standard I/O library
#include <string.h>     // String manipulation functions
#include <stdlib.h>     // Standard library functions
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for internet addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls

#define PORTNO 10200    // Port number for server connection
#define OFFSET 4        // Offset value for decryption

int main() {
    int server_fd, newsockfd;              // Socket descriptors
    struct sockaddr_in address;            // Structure for server address
    socklen_t addrlen;                     // Length of client address structure
    char encrypted_message[256];           // Buffer for encrypted message
    char decrypted_message[256];           // Buffer for decrypted message

    // Create a TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Define server address parameters
    address.sin_family = AF_INET;          // Address family (IPv4)
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP
    address.sin_port = htons(PORTNO);      // Port number

    // Bind the socket to the IP and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }
    printf("Server waiting for client connection...\n");

    // Accept a client connection
    addrlen = sizeof(address);
    newsockfd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (newsockfd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(1);
    }

    // Read the encrypted message from the client
    int bytes_read = read(newsockfd, encrypted_message, sizeof(encrypted_message) - 1);
    if (bytes_read < 0) {
        perror("Read failed");
        close(newsockfd);
        close(server_fd);
        exit(1);
    }
    encrypted_message[bytes_read] = '\0'; // Null-terminate the encrypted message

    printf("Encrypted message received: %s\n", encrypted_message);

    // Decrypt the message by subtracting OFFSET from each character
    int len = strlen(encrypted_message);
    for (int i = 0; i < len; i++) {
        decrypted_message[i] = encrypted_message[i] - OFFSET;
    }
    decrypted_message[len] = '\0'; // Null-terminate the decrypted message

    // Display the decrypted message
    printf("Decrypted message: %s\n", decrypted_message);

    close(newsockfd); // Close the client socket
    close(server_fd); // Close the server socket
    return 0;
}
