/*
Question:
Implement a simple client-server communication program using TCP in C.
- The client should send a string to the server.
- The server will receive this string and echo it back to the client.
- The communication should terminate after one exchange, and the server should handle multiple clients sequentially.
- The client will print the server’s response.
*/

/* 
client.c
- This program creates a TCP client that connects to the server on localhost and port 10200.
- It prompts the user to enter a string, sends this string to the server, and receives a response.
- The client displays the server's response and then closes the connection.
*/

// client.c code
#include <stdio.h>      // Standard input-output library
#include <string.h>     // String manipulation library
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for storing addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API for UNIX system calls
#define PORTNO 10200    // Port number for server connection

int main() {
    int sockfd, n;                          // sockfd: Socket file descriptor, n: for reading length
    struct sockaddr_in server_addr;         // Structure to store server address
    char buf[256];                          // Buffer for server response
    char ch[256];                           // Buffer for client message

    // Creating a socket (IPv4, TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");   // Error handling for socket creation
        exit(EXIT_FAILURE);                 // Exit if socket creation fails
    }

    // Defining server address parameters
    server_addr.sin_family = AF_INET;       // Address family (IPv4)
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost IP
    server_addr.sin_port = htons(PORTNO);   // Port number in network byte order

    // Connecting to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");        // Error handling for connection
        close(sockfd);                      // Close socket on failure
        exit(EXIT_FAILURE);
    }

    // Prompting user to enter a string to send to the server
    printf("Enter string to send: ");
    fgets(ch, sizeof(ch), stdin);           // Read input from user
    ch[strcspn(ch, "\n")] = '\0';           // Remove newline character

    // Sending message to the server
    if (write(sockfd, ch, strlen(ch)) < 0) {
        perror("write failed");             // Error handling for write
        close(sockfd);                      // Close socket on failure
        exit(EXIT_FAILURE);
    }

    // Reading server response
    memset(buf, 0, sizeof(buf));            // Clear buffer
    n = read(sockfd, buf, sizeof(buf) - 1); // Read response from server
    if (n < 0) {
        perror("read failed");              // Error handling for read
        close(sockfd);                      // Close socket on failure
        exit(EXIT_FAILURE);
    }
    buf[n] = '\0';                          // Null-terminate the response
    printf("Response from server: %s\n", buf); // Display server response

    close(sockfd);                          // Close the socket after communication ends
    return 0;                               // Exit program
}

/* 
server.c
- This program creates a TCP server that listens on port 10200 for incoming client connections.
- For each client, it reads the message sent by the client, prints it, and sends it back.
- The server can handle multiple clients sequentially.
- The connection is closed after each client exchange, and the server waits for the next client.
*/

// server.c code
#include <stdio.h>      // Standard input-output library
#include <string.h>     // String manipulation library
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for storing addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <stdlib.h>     // Standard library functions
#include <unistd.h>     // POSIX API for UNIX system calls
#define PORTNO 10200    // Port number for server connection

int main() {
    int sockfd, newsockfd, clilen, n;       // Socket descriptors and reading length
    struct sockaddr_in seraddr, cliaddr;    // Structures for server and client addresses
    char buf[256];                          // Buffer to store client message
    socklen_t addrlen;                      // Length of client address structure

    // Creating a socket (IPv4, TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");   // Error handling for socket creation
        exit(EXIT_FAILURE);                 // Exit if socket creation fails
    }

    // Defining server address parameters
    seraddr.sin_family = AF_INET;           // Address family (IPv4)
    seraddr.sin_addr.s_addr = INADDR_ANY;   // Accept connections from any IP address
    seraddr.sin_port = htons(PORTNO);       // Port number in network byte order

    // Binding the socket to the specified IP and port
    if (bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0) {
        perror("bind failed");              // Error handling for bind
        close(sockfd);                      // Close socket on failure
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections, with a maximum backlog of 5 connections
    if (listen(sockfd, 5) < 0) {
        perror("listen failed");            // Error handling for listen
        close(sockfd);                      // Close socket on failure
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for connections...\n");

    // Infinite loop to handle multiple clients
    while (1) {
        addrlen = sizeof(cliaddr);          // Set length of client address structure

        // Accepting a client connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &addrlen);
        if (newsockfd < 0) {
            perror("accept failed");        // Error handling for accept
            continue;                       // Continue loop to accept other clients
        }

        // Reading message from client
        memset(buf, 0, sizeof(buf));        // Clear buffer
        n = read(newsockfd, buf, sizeof(buf) - 1);
        if (n < 0) {
            perror("read failed");          // Error handling for read
            close(newsockfd);               // Close socket on failure
            continue;
        }

        buf[n] = '\0';                      // Null-terminate the client message
        printf("Message from Client: %s\n", buf); // Display client message

        // Echoing message back to the client
        n = write(newsockfd, buf, strlen(buf));
        if (n < 0) {
            perror("write failed");         // Error handling for write
        }
        close(newsockfd);                   // Close client socket after response
    }
    close(sockfd);                          // Close server socket (never reached in infinite loop)
    return 0;                               // Exit program
}
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
}/*
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
}/*
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
/*
client.c
- This program creates a TCP client that connects to a web server on a specified IP and port (usually 80 for HTTP).
- The client sends a manually crafted HTTP GET request and receives the HTTP response, displaying all headers and content.
*/

#include <stdio.h>      // Standard I/O library
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String manipulation functions
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for internet addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls

#define PORT 80         // Port number for HTTP requests (default for HTTP)
#define BUFFER_SIZE 4096 // Size of buffer to store the response

int main() {
    int sockfd;                          // Socket descriptor
    struct sockaddr_in server_addr;      // Structure for server address
    char request[512];                   // Buffer for HTTP request
    char response[BUFFER_SIZE];          // Buffer for HTTP response
    char *server_ip = "93.184.216.34";   // Example server IP (example.com); replace with actual IP if needed

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Define server address parameters
    server_addr.sin_family = AF_INET;                // Address family (IPv4)
    server_addr.sin_port = htons(PORT);              // Port number (HTTP)
    server_addr.sin_addr.s_addr = inet_addr(server_ip); // Server IP address (replace with actual IP if needed)

    // Connect to the web server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(1);
    }

    // Create a manually crafted HTTP GET request for the root page
    snprintf(request, sizeof(request), 
             "GET / HTTP/1.1\r\n"                  // HTTP method and path
             "Host: example.com\r\n"               // Host header (replace with actual domain if needed)
             "Connection: close\r\n"               // Connection header to close connection after response
             "\r\n");

    // Send the HTTP request to the server
    send(sockfd, request, strlen(request), 0);
    printf("HTTP request sent:\n%s\n", request);

    // Receive the HTTP response from the server and display all fields
    int bytes_received;
    printf("HTTP response received:\n");
    while ((bytes_received = read(sockfd, response, sizeof(response) - 1)) > 0) {
        response[bytes_received] = '\0';           // Null-terminate the response
        printf("%s", response);                    // Display the response
    }

    if (bytes_received < 0) {
        perror("Read failed");
    }

    close(sockfd);                                 // Close the socket
    return 0;
}
/*
Question:
Implement a simple TCP client-server program in C where:
- The server listens on a specified port, accepts connections, and echoes back messages received from the client.
- The client sends a message to the server, receives the echoed message, and displays it.
Note: Replace the IP address in both the server and client with your system's IP address.
*/

/*
server.c
- This program creates a TCP server that listens on IP 172.16.59.10 and port 10200.
- For each connected client, it forks a child process to handle communication.
- The server reads a message from the client and echoes it back.
*/

#include <stdio.h>      // Standard I/O library
#include <string.h>     // String manipulation functions
#include <sys/types.h>  // Data types used in system calls
#include <sys/socket.h> // Socket API
#include <netinet/in.h> // Structures for internet addresses
#include <unistd.h>     // POSIX API for UNIX system calls
#include <stdlib.h>     // Standard library functions

#define PORTNO 10200    // Port number for the server

int main() {
    int sockfd, newsockfd, clilen, n;
    char buf[256];                             // Buffer to store client message
    struct sockaddr_in seraddr, cliaddr;       // Structures for server and client addresses

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address parameters
    seraddr.sin_family = AF_INET;              // Address family (IPv4)
    seraddr.sin_addr.s_addr = inet_addr("172.16.59.10"); // Server IP (replace with your IP)
    seraddr.sin_port = htons(PORTNO);          // Port number in network byte order

    // Bind the socket to the specified IP and port
    if (bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections, with a maximum backlog of 5 connections
    listen(sockfd, 5);
    printf("Server is listening for incoming connections...\n");

    // Infinite loop to accept and handle clients
    while (1) {
        clilen = sizeof(cliaddr);

        // Accept an incoming client connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
        if (newsockfd < 0) {
            perror("Accept failed");
            continue;
        }

        // Fork a child process to handle the connected client
        if (fork() == 0) {
            // Child process: read from and write to the client
            n = read(newsockfd, buf, sizeof(buf));
            if (n < 0) {
                perror("Read failed");
                close(newsockfd);
                exit(EXIT_FAILURE);
            }
            printf("\nMessage from Client: %s\n", buf);

            // Echo back the received message
            n = write(newsockfd, buf, sizeof(buf));
            if (n < 0) {
                perror("Write failed");
                close(newsockfd);
                exit(EXIT_FAILURE);
            }

            close(newsockfd);  // Close the client socket
            exit(0);            // Exit the child process
        } else {
            // Parent process: close the socket for this client and continue to accept new clients
            close(newsockfd);
        }
    }

    close(sockfd); // Close the server socket (not reached in this code)
    return 0;      // Exit program
}
/*
client.c
- This program creates a TCP client that connects to a server on IP 172.16.56.10 and port 9704.
- It prompts the user to enter a message, sends this message to the server, and receives the echoed message back.
*/

#include <stdio.h>      // Standard I/O library
#include <stdlib.h>     // Standard library functions
#include <fcntl.h>      // File control options
#include <string.h>     // String manipulation functions
#include <sys/socket.h> // Socket API
#include <sys/types.h>  // Data types used in system calls
#include <netinet/in.h> // Structures for internet addresses
#include <arpa/inet.h>  // Definitions for internet operations
#include <unistd.h>     // POSIX API for UNIX system calls

#define PORT 9704       // Port number for server connection

int main() {
    int sd;                                 // Socket descriptor
    struct sockaddr_in address;             // Structure for server address

    // Create a TCP socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address parameters
    address.sin_family = AF_INET;                           // Address family (IPv4)
    address.sin_addr.s_addr = inet_addr("172.16.56.10");    // Server IP (replace with your IP)
    address.sin_port = htons(PORT);                         // Port number in network byte order

    // Connect to the server
    if (connect(sd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Connection failed");
        close(sd);
        exit(EXIT_FAILURE);
    }

    // Prompt user for input
    char buf[25];
    printf("Enter a message: ");
    fgets(buf, sizeof(buf), stdin);                         // Read message from user

    // Send the message to the server
    int m = send(sd, buf, strlen(buf), 0);
    if (m < 0) {
        perror("Send failed");
        close(sd);
        exit(EXIT_FAILURE);
    }

    // Receive the echoed message from the server
    int n = recv(sd, buf, sizeof(buf), 0);
    if (n < 0) {
        perror("Receive failed");
        close(sd);
        exit(EXIT_FAILURE);
    }
    
    printf("The server echoed: %s\n", buf);                 // Display echoed message

    close(sd);                                              // Close the socket
    return 0;                                               // Exit program
}
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
