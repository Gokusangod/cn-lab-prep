/*
Question:
Create a TCP peer-to-peer program in C where:
- The user can choose to run the program as a client or a server.
- The client sends a string to the server.
- The server receives the string, reverses it, and sends the reversed string back to the client.
*/

/*
Description:
- The program defines functions for creating sockets, binding, connecting, sending, and receiving data.
- The `peer_server` function listens for incoming connections and processes the received string.
- The `peer_client` function prompts the user to enter a string, sends it to the server, and receives the reversed string back.
*/

#include <stdio.h>      // Standard I/O library
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String manipulation functions
#include <unistd.h>     // POSIX API for UNIX system calls
#include <netinet/in.h> // Structures for internet addresses
#include <sys/socket.h> // Socket API
#include <arpa/inet.h>  // Definitions for internet operations
#include <sys/types.h>  // Data types used in system calls

#define PORT 10320      // Port number for the server

// Function to reverse a given string
void reverseString(char str[]) {
    int n = strlen(str);
    for (int i = 0; i < n / 2; i++) {
        char temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
}

// Function to create a TCP socket and handle errors
int create_socket() {
    int socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return socket_id;
}

// Function to bind socket to the specified address and port
void bind_socket(int socket_id, struct sockaddr_in *address) {
    if (bind(socket_id, (struct sockaddr *)address, sizeof(*address)) == -1) {
        perror("Bind failed");
        close(socket_id);
        exit(EXIT_FAILURE);
    }
}

// Function to connect the client to the server
void connect_to_peer(int socket_id, struct sockaddr_in *address) {
    if (connect(socket_id, (struct sockaddr *)address, sizeof(*address)) == -1) {
        perror("Connection failed");
        close(socket_id);
        exit(EXIT_FAILURE);
    }
}

// Function to send data through the socket
void send_data(int socket_id, char *data) {
    write(socket_id, data, strlen(data) + 1);
}

// Function to receive data through the socket
void receive_data(int socket_id, char *buffer, size_t size) {
    read(socket_id, buffer, size);
}

// Server function to accept connections and handle string reversal
void peer_server(int socket_id) {
    struct sockaddr_in clientaddress;
    socklen_t socklen = sizeof(clientaddress);

    // Accept an incoming connection from a client
    int new_socket_id = accept(socket_id, (struct sockaddr *)&clientaddress, &socklen);
    if (new_socket_id == -1) {
        perror("Accept failed");
        close(socket_id);
        exit(EXIT_FAILURE);
    }

    // Buffer to store the received string
    char buffer[256];
    receive_data(new_socket_id, buffer, sizeof(buffer));
    printf("Received string: %s\n", buffer);

    // Reverse the received string
    reverseString(buffer);
    printf("Reversed string: %s\n", buffer);

    // Send the reversed string back to the client
    send_data(new_socket_id, buffer);
    printf("Reversed string sent to peer.\n");

    close(new_socket_id); // Close the client socket after communication
}

// Client function to send string and receive the reversed string from the server
void peer_client(int socket_id) {
    char buff[256];
    printf("Enter a string: ");
    scanf("%s", buff);  // Read input string from the user

    // Send the input string to the server
    send_data(socket_id, buff);
    printf("Sent string to the peer.\n");

    // Buffer to store the reply from the server
    char reply[256];
    receive_data(socket_id, reply, sizeof(reply));
    printf("Peer's reply: %s\n", reply);

    close(socket_id); // Close the socket after communication
}

// Main function where the user selects to run as client or server
int main() {
    int role;
    printf("Choose your role (1 for Client, 2 for Server): ");
    scanf("%d", &role);

    int socket_id = create_socket(); // Create a socket

    struct sockaddr_in address;      // Structure to define address
    address.sin_family = AF_INET;    // Set address family to IPv4
    address.sin_port = htons(PORT);  // Set port number

    // If user selects Client role
    if (role == 1) {
        address.sin_addr.s_addr = inet_addr("127.0.0.1");  // Set server IP address (localhost)
        connect_to_peer(socket_id, &address);              // Connect to server
        peer_client(socket_id);                            // Start client process

    // If user selects Server role
    } else if (role == 2) {
        address.sin_addr.s_addr = inet_addr("0.0.0.0");    // Bind to any IP address
        bind_socket(socket_id, &address);                  // Bind socket to address

        listen(socket_id, 5);                              // Listen for incoming connections
        printf("Listening for incoming connections...\n");

        peer_server(socket_id);                            // Start server process
    } else {
        printf("Invalid role selection. Please choose 1 (Client) or 2 (Server).\n");
    }
    return 0;                                              // Exit program
}
