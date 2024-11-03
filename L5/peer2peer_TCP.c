#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define PORT 10320

void reverseString(char str[]) {
    int n = strlen(str);
    for (int i = 0; i < n / 2; i++) {
        char temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
}

int create_socket() {
    int socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return socket_id;
}

void bind_socket(int socket_id, struct sockaddr_in *address) {
    if (bind(socket_id, (struct sockaddr *)address, sizeof(*address)) == -1) {
        perror("Bind failed");
        close(socket_id);
        exit(EXIT_FAILURE);
    }
}

void connect_to_peer(int socket_id, struct sockaddr_in *address) {
    if (connect(socket_id, (struct sockaddr *)address, sizeof(*address)) == -1) {
        perror("Connection failed");
        close(socket_id);
        exit(EXIT_FAILURE);
    }
}

void send_data(int socket_id, char *data) {
    write(socket_id, data, strlen(data) + 1);
}

void receive_data(int socket_id, char *buffer, size_t size) {
    read(socket_id, buffer, size);
}

void peer_server(int socket_id) {
    struct sockaddr_in clientaddress;
    socklen_t socklen = sizeof(clientaddress);
    int new_socket_id = accept(socket_id, (struct sockaddr *)&clientaddress, &socklen);

    if (new_socket_id == -1) {
        perror("Accept failed");
        close(socket_id);
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    receive_data(new_socket_id, buffer, sizeof(buffer));
    printf("Received string: %s\n", buffer);

    reverseString(buffer);
    printf("Reversed string: %s\n", buffer);

    send_data(new_socket_id, buffer);
    printf("Reversed string sent to peer.\n");

    close(new_socket_id);
}

void peer_client(int socket_id) {
    char buff[256];
    printf("Enter a string: ");
    scanf("%s", buff);

    send_data(socket_id, buff);
    printf("Sent string to the peer.\n");

    char reply[256];
    receive_data(socket_id, reply, sizeof(reply));
    printf("Peer's reply: %s\n", reply);

    close(socket_id);
}

int main() {

  int role;
    printf("Choose your role (1 for Client, 2 for Server): ");
    scanf("%d", &role);

    int socket_id = create_socket();

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (role == 1) {
        address.sin_addr.s_addr = inet_addr("127.0.0.1");
        connect_to_peer(socket_id, &address);
        peer_client(socket_id);
    } else if (role == 2) {

      address.sin_addr.s_addr = inet_addr("0.0.0.0");
        bind_socket(socket_id, &address);

        listen(socket_id, 5);
        printf("Listening for incoming connections...\n");

        peer_server(socket_id);
    } else {
        printf("Invalid role selection. Please choose 1 (Client) or 2 (Server).\n");
    }
    return 0;
}
