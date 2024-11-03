/*
Question:
Implement a TCP client-server program in C where:
- The client sends a string to the server.
- The server removes duplicate characters from the string and sends the modified string back.
- If the client sends the string "Stop", both client and server programs terminate.
*/

/*
client.c
- This program creates a TCP client that connects to a server on IP 192.168.10.10 and port 10200.
- It prompts the user to enter a string, sends this string to the server, and receives the modified string without duplicate characters.
- The client exits when the input string is "Stop".
*/

#include <stdio.h>      // Standard input-output library
#include <unistd.h>     // POSIX API for UNIX system calls
#include <string.h>     // String manipulation library
#include <arpa/inet.h>  // Definitions for internet operations
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for storing addresses
#include <stdlib.h>     // Standard library functions
#define PORTNO 10250    // Port number for server connection

int main() {
    char buff[256];               // Buffer to store server response
    int n = 1;                    // Loop control variable

    while (1) {
        int socket_id = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
        struct sockaddr_in address;                      // Structure to store server address

        address.sin_family = AF_INET;                    // Address family (IPv4)
        address.sin_addr.s_addr = inet_addr("192.168.10.10"); // Server IP
        address.sin_port = htons(10200);                 // Port number in network byte order

        // Connecting to the server
        int result = connect(socket_id, (struct sockaddr*)&address, sizeof(address));
        if (result == -1) {
            perror("\nClient Error");                   // Error handling for connection
            exit(1);
        }
        
        // Prompting user for input
        char str[256];
        printf("\nEnter String: ");
        scanf("%s", str);

        // Exit condition when the user inputs "Stop"
        if (strcmp(str, "Stop") == 0)
            exit(1);

        // Send input string to server
        write(socket_id, str, sizeof(str));

        // Receive and display server's response
        read(socket_id, buff, sizeof(buff));
        printf("The edited string with removed duplicates is: ");
        puts(buff);

        // Close the socket after each transaction
        close(socket_id);
    }
}
/*
server.c
- This program creates a TCP server that listens on IP 192.168.10.10 and port 10250.
- For each connected client, it reads the client's string, removes duplicate characters, and sends back the modified string.
- If the client sends the string "Stop", the server terminates.
*/

#include <stdio.h>      // Standard input-output library
#include <unistd.h>     // POSIX API for UNIX system calls
#include <string.h>     // String manipulation library
#include <arpa/inet.h>  // Definitions for internet operations
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for storing addresses
#include <stdlib.h>     // Standard library functions
#define PORTNO 10250    // Port number for server connection

// Function to check if character is already present in 'character' array
int isPresent(char ch, char character[], int index) {
    for (int i = 0; i < strlen(character); i++) {
        if (i == index)
            continue;
        else if (ch == character[i])
            return 1;              // Return 1 if character is duplicate
    }
    return 0;                      // Return 0 if character is unique
}

// Function to count occurrences of a character in a buffer
int count(char ch, char buffer[]) {
    int occ = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        if (ch == buffer[i])
            occ++;
    }
    return occ;
}

int main() {
    printf("Server running...\n");
    int socket_id = socket(AF_INET, SOCK_STREAM, 0);     // Create TCP socket
    struct sockaddr_in serveraddress;                    // Structure for server address

    serveraddress.sin_family = AF_INET;                  // Address family (IPv4)
    serveraddress.sin_addr.s_addr = inet_addr("192.168.10.10"); // Server IP
    serveraddress.sin_port = htons(PORTNO);              // Port number in network byte order

    // Bind the socket to the IP and port
    bind(socket_id, (struct sockaddr *)&serveraddress, sizeof(serveraddress));

    // Listen for incoming connections
    listen(socket_id, 5);

    while (1) {
        char buffer[256], result[256], character[256];   // Buffers for message processing
        int unique_count = 0, frequency[256];            // Unique character count and frequency array

        struct sockaddr_in clientaddress;                // Structure for client address
        socklen_t client_len = sizeof(clientaddress);    // Length of client address structure

        // Accept client connection
        int new_socket_id = accept(socket_id, (struct sockaddr *)&clientaddress, &client_len);
        
        // Read message from client
        read(new_socket_id, buffer, sizeof(buffer));
        if (strcmp(buffer, "Stop") == 0)                 // If client sends "Stop", exit server
            break;
        
        // Initialize arrays for processing
        for (int i = 0; i < 256; i++) {
            frequency[i] = 0;
            character[i] = '\0';
            result[i] = '\0';
        }

        // Remove duplicates by checking presence and counting occurrences
        for (int i = 0; i < strlen(buffer); i++) {
            if (isPresent(buffer[i], character, i)) {    // Check if character is duplicate
                character[i] = '$';                      // Mark duplicate with special character
                frequency[i] = -1;                       // Set frequency to -1 for duplicates
            } else {
                frequency[i] = count(buffer[i], buffer); // Count occurrences of character
                character[i] = buffer[i];                // Store unique character
                result[unique_count] = character[i];     // Append to result string
                unique_count++;                          // Increment unique count
            }
        }

        // Send modified string back to client
        write(new_socket_id, result, sizeof(result));
        
        // Close the client socket after response
        close(new_socket_id);
    }

    // Close the server socket after termination
    close(socket_id);
    return 0;
}
