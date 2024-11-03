/*
Question:
Implement a simple UDP client-server program in C where:
- The client sends a message to the server.
- The server receives the message and echoes it back to the client.
- Both programs use UDP for connectionless communication.
Note: Replace "172.16.56.10" with your system's IP address.
*/

/*
server.c
- This program creates a UDP server that listens on IP 172.16.56.10 and port 9704.
- It waits to receive a message from a client, displays the message, and then sends the same message back to the client.
*/

#include <stdio.h>      // Standard input-output library
#include <fcntl.h>      // File control options
#include <stdlib.h>     // Standard library functions
#include <sys/socket.h> // Socket API
#include <sys/types.h>  // Data types used in system calls
#include <netinet/in.h> // Structures for storing addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls

int main() {
    int sd;                            // Socket descriptor
    char buf[25];                      // Buffer to store received message
    struct sockaddr_in sadd, cadd;     // Structures for server and client addresses

    // Create a UDP socket
    sd = socket(AF_INET, SOCK_DGRAM, 0);

    // Define server address parameters
    sadd.sin_family = AF_INET;                      // Address family (IPv4)
    sadd.sin_addr.s_addr = inet_addr("172.16.56.10"); // Server IP (replace with your IP)
    sadd.sin_port = htons(9704);                    // Port number in network byte order

    // Bind the socket to the specified IP and port
    int result = bind(sd, (struct sockaddr *)&sadd, sizeof(sadd));

    int len = sizeof(cadd);               // Length of client address structure

    // Receive a message from the client
    int m = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&cadd, &len);
    printf("The server received: \n");
    puts(buf);                            // Display received message

    // Send the same message back to the client
    int n = sendto(sd, buf, sizeof(buf), 0, (struct sockaddr *)&cadd, len);

    return 0;                             // Exit program
}
/*
client.c
- This program creates a UDP client that sends a message to a server on IP 172.16.56.10 and port 9704.
- It takes input from the user, sends it to the server, and then receives and displays the echoed message from the server.
*/

#include <stdio.h>      // Standard input-output library
#include <stdlib.h>     // Standard library functions
#include <fcntl.h>      // File control options
#include <sys/socket.h> // Socket API
#include <sys/types.h>  // Data types used in system calls
#include <netinet/in.h> // Structures for storing addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls

int main() {
    int sd;                            // Socket descriptor
    struct sockaddr_in address;        // Structure for server address
    char buf[25], buf1[25];            // Buffers for sending and receiving messages

    // Create a UDP socket
    sd = socket(AF_INET, SOCK_DGRAM, 0);

    // Define server address parameters
    address.sin_family = AF_INET;                     // Address family (IPv4)
    address.sin_addr.s_addr = inet_addr("172.16.56.10"); // Server IP (replace with your IP)
    address.sin_port = htons(9704);                   // Port number in network byte order

    // Prompt user for input
    printf("Enter message to send: ");
    fgets(buf, sizeof(buf), stdin);                   // Read input from user

    int len = sizeof(address);                        // Length of server address structure

    // Send message to the server
    int m = sendto(sd, buf, sizeof(buf), 0, (struct sockaddr *)&address, len);

    // Receive echoed message from the server
    int n = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&address, &len);
    printf("The server echo is: \n");
    puts(buf);                                       // Display echoed message

    return 0;                                        // Exit program
}
