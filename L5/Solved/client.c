#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#define PORTNO 10200

int main() {
    int sockfd, n;
    struct sockaddr_in server_addr;
    char buf[256];
    char ch[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORTNO);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Enter string to send: ");
    fgets(ch, sizeof(ch), stdin);
    ch[strcspn(ch, "\n")] = '\0';

    if (write(sockfd, ch, strlen(ch)) < 0) {
        perror("write failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    memset(buf, 0, sizeof(buf));
    n = read(sockfd, buf, sizeof(buf) - 1);
    if (n < 0) {
        perror("read failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    buf[n] = '\0';
    printf("Response from server: %s\n", buf);
    close(sockfd);
    return 0;
}
