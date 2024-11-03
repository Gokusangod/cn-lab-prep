/*
Question:
Implement a UDP client-server program in C where:
- The client sends a 3x3 matrix row-by-row to the server.
- The server receives the rows, reconstructs the matrix, and displays it.
- The programs use UDP for connectionless communication.
*/

/*
client.c
- This program creates a UDP client that sends a 3x3 matrix row-by-row to a server on IP 127.0.0.1 and port 10203.
- It takes matrix input from the user, formats each row as a string, and sends each row separately to the server.
*/

#include <stdio.h>      // Standard input-output library
#include <stdlib.h>     // Standard library functions
#include <fcntl.h>      // File control options
#include <string.h>     // String manipulation library
#include <sys/socket.h> // Socket API
#include <sys/types.h>  // Data types used in system calls
#include <netinet/in.h> // Structures for storing addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls

#define PORT 10203      // Port number for server connection
#define ROWS 3          // Number of rows in matrix
#define COLS 3          // Number of columns in matrix

int main() {
    int sd;                               // Socket descriptor
    struct sockaddr_in address;           // Structure for server address
    int matrix[ROWS][COLS];               // 3x3 matrix
    sd = socket(AF_INET, SOCK_DGRAM, 0);  // Create UDP socket
    if(sd < 0) {
        perror("Socket creation failed"); // Error handling for socket creation
        exit(EXIT_FAILURE);
    }
    
    // Define server address parameters
    address.sin_family = AF_INET;                          // Address family (IPv4)
    address.sin_addr.s_addr = inet_addr("127.0.0.1");      // Server IP (localhost)
    address.sin_port = htons(PORT);                        // Port number in network byte order
    
    // Prompt user for matrix input
    printf("Enter a %dx%d matrix (row-wise):\n", ROWS, COLS);
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            scanf("%d", &matrix[i][j]);                    // Read each element of the matrix
        }
    }
    
    int len = sizeof(address);                             // Length of server address structure
    
    // Send each row of the matrix to the server
    for(int i = 0; i < ROWS; i++) {
        char buf[100];
        memset(buf, 0, sizeof(buf));                       // Clear buffer
        sprintf(buf, "%d %d %d", matrix[i][0], matrix[i][1], matrix[i][2]); // Format row as string
        int m = sendto(sd, buf, sizeof(buf), 0, (struct sockaddr *)&address, len); // Send row to server
        if(m < 0) {
            perror("Send failed");                         // Error handling for send
            close(sd);
            exit(EXIT_FAILURE);
        }
        printf("Sent row %d: %s\n", i + 1, buf);           // Confirm row sent
    }
    close(sd);                                             // Close the socket after sending all rows
    return 0;                                              // Exit program
}
/*
server.c
- This program creates a UDP server that listens on IP 127.0.0.1 and port 10203.
- It waits to receive each row of a 3x3 matrix from a client, reconstructs the matrix, and displays it.
*/

#include <stdio.h>      // Standard input-output library
#include <fcntl.h>      // File control options
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String manipulation library
#include <sys/socket.h> // Socket API
#include <sys/types.h>  // Data types used in system calls
#include <netinet/in.h> // Structures for storing addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls

#define PORT 10203      // Port number for server connection
#define ROWS 3          // Number of rows in matrix
#define COLS 3          // Number of columns in matrix
#define BUFSIZE 100     // Buffer size for receiving data

int main() {
    int sd;                             // Socket descriptor
    char buf[BUFSIZE];                  // Buffer to store received message
    struct sockaddr_in sadd, cadd;      // Structures for server and client addresses
    int matrix[ROWS][COLS];             // 3x3 matrix
    int row_counter = 0;                // Counter for received rows
    int len = sizeof(cadd);             // Length of client address structure

    sd = socket(AF_INET, SOCK_DGRAM, 0); // Create UDP socket
    if (sd < 0) {
        perror("Socket creation failed"); // Error handling for socket creation
        exit(EXIT_FAILURE);
    }

    // Define server address parameters
    sadd.sin_family = AF_INET;                          // Address family (IPv4)
    sadd.sin_addr.s_addr = inet_addr("127.0.0.1");      // Server IP (localhost)
    sadd.sin_port = htons(PORT);                        // Port number in network byte order

    // Bind the socket to the specified IP and port
    if (bind(sd, (struct sockaddr *)&sadd, sizeof(sadd)) < 0) {
        perror("Bind failed");                         // Error handling for bind
        close(sd);
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting...\n");

    // Loop to receive each row of the matrix from the client
    while (row_counter < ROWS) {
        memset(buf, 0, BUFSIZE);                       // Clear buffer

        // Receive a row from the client
        int m = recvfrom(sd, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&cadd, &len);
        if (m < 0) {
            perror("Receive failed");                  // Error handling for receive
            close(sd);
            exit(EXIT_FAILURE);
        }
        buf[m] = '\0';                                 // Null-terminate received string

        // Parse the received row and store in matrix
        int i = 0;
        char *token = strtok(buf, " ");                // Split string by spaces
        while (token != NULL && i < COLS) {
            matrix[row_counter][i] = atoi(token);      // Convert token to integer and store
            token = strtok(NULL, " ");
            i++;
        }
        
        if (i != COLS) {                               // Error handling for incomplete row
            fprintf(stderr, "Received incomplete row: %s\n", buf);
            close(sd);
            exit(EXIT_FAILURE);
        }
        
        row_counter++;                                 // Move to the next row
    }

    // Display the reconstructed matrix
    printf("The received matrix is:\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    if (close(sd) < 0) {                               // Close the socket after receiving all rows
        perror("Close failed");
        exit(EXIT_FAILURE);
    }

    return 0;                                          // Exit program
}
