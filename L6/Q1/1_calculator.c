/*
Question:
Create a TCP client-server calculator program in C where:
- The client sends two operands and an operator (addition, subtraction, multiplication, division, or modulus) to the server.
- The server performs the operation and returns the result to the client.
- The client displays the result.
*/

/*
client.c
- This program creates a TCP client that connects to a server on IP 127.0.0.1 and port 10202.
- The client takes two operands and an operator choice from the user, sends them to the server, and receives the calculated result.
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
int num[3] = {0};                       // Array to store operands and operator choice
char result[100] = {0};                 // Buffer for result received from server

// Function to create and configure the client socket
void CreateClientSocket() {
    sock = socket(AF_INET, SOCK_STREAM, 0);           // Create a TCP socket
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;                     // Address family (IPv4)
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address
    address.sin_port = htons(PORTNO);                 // Port number
    addrlen = sizeof(address);                        // Address length
}

// Function to connect to the server and send data
void PerformClientTask() {
    client_fd = connect(sock, (struct sockaddr *)&address, addrlen);
    if (client_fd == -1) {
        perror("\nCLIENT ERROR");
        exit(EXIT_FAILURE);
    }

    // Input operands and operator choice
    printf("Enter first operand: ");
    scanf("%d", &(num[0]));
    printf("1: +\t2: -\t3: *\t4: /\t5: %%\nEnter choice: ");
    scanf("%d", &(num[1]));
    printf("Enter second operand: ");
    scanf("%d", &(num[2]));

    // Send operands and operator to the server
    send(sock, num, sizeof(num), 0);
    printf("Operators and operands sent.\n");

    // Receive and display the result from the server
    valread = read(sock, &result, sizeof(result));
    printf("%s\n", result);
}

int main() {
    CreateClientSocket();  // Create and configure client socket
    PerformClientTask();   // Perform client task (send data and receive result)
    close(client_fd);      // Close the connection
    return 0;              // Exit program
}
/*
server.c
- This program creates a TCP server that listens on IP 127.0.0.1 and port 10202.
- For each connected client, it forks a child process to handle the client's calculation request.
- The server receives operands and operator choice, performs the calculation, and sends the result back to the client.
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
int num[3] = {0};                       // Array to store operands and operator choice from client
float result = 0;                       // Variable to store calculation result
char msg[100];                          // Message buffer to send the result to client

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

// Function to handle client requests for calculation
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
            // Child process: receive data from client
            valread = read(new_socket, num, sizeof(num));
            if (valread < 0) {
                perror("Read failed");
                close(new_socket);
                exit(EXIT_FAILURE);
            }

            // Perform the calculation based on the operator choice
            if (num[1] == 1)
                result = num[0] + num[2];
            else if (num[1] == 2)
                result = num[0] - num[2];
            else if (num[1] == 3)
                result = num[0] * num[2];
            else if (num[1] == 4)
                result = (float)num[0] / num[2];
            else if (num[1] == 5)
                result = num[0] % num[2];

            // Display the calculated result
            printf("\nOperation performed.\n");
            printf("Result calculated: %0.2f\n", result);

            // Prepare and send the result message back to client
            memset(msg, 0, sizeof(msg));
            sprintf(msg, "The result of the operation is: %0.2f", result);
            send(new_socket, &msg, sizeof(msg), 0);

            close(new_socket);  // Close the client socket
            exit(0);            // Exit the child process
        } else {
            // Parent process: close the client socket and continue to accept new clients
            close(new_socket);
        }
    }
}

int main() {
    CreateServerSocket();        // Create and configure server socket
    PerformServerTask();         // Perform server task (handle client requests)
    shutdown(server_fd, SHUT_RDWR);  // Shutdown the server
    return 0;                    // Exit program
}
