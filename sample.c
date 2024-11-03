#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Simple TCP Echo Server
void tcp_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 1); // Listen for 1 connection

    int client_fd = accept(server_fd, NULL, NULL); // Accept client connection
    char buffer[BUFFER_SIZE];
    int bytes_read = read(client_fd, buffer, BUFFER_SIZE); // Read message from client
    send(client_fd, buffer, bytes_read, 0); // Echo message back to client

    close(client_fd); // Close client connection
    close(server_fd); // Close server
}

// Simple TCP Echo Client
void tcp_client(const char *server_ip) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &server_address.sin_addr);

    connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)); // Connect to server

    char message[] = "Hello, Server!";
    send(sock, message, strlen(message), 0); // Send message to server

    char buffer[BUFFER_SIZE];
    int bytes_received = read(sock, buffer, BUFFER_SIZE); // Receive echoed message
    buffer[bytes_received] = '\0'; // Null-terminate received message

    close(sock); // Close client connection
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <server/client> [server_ip]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "server") == 0) {
        tcp_server();
    } else if (strcmp(argv[1], "client") == 0 && argc == 3) {
        tcp_client(argv[2]);
    } else {
        printf("Invalid usage\n");
    }

    return 0;
}