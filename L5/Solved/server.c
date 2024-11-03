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
    int sockfd, newsockfd, clilen, n;
    struct sockaddr_in seraddr, cliaddr;
    char buf[256];
    socklen_t addrlen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    seraddr.sin_family = AF_INET;
   seraddr.sin_addr.s_addr = INADDR_ANY;
    seraddr.sin_port = htons(PORTNO);

    if (bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0) {
        perror("listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for connections...\n");

    while (1) {
        addrlen = sizeof(cliaddr);

        newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &addrlen);
        if (newsockfd < 0) {
            perror("accept failed");
            continue;
        }

        memset(buf, 0, sizeof(buf));
        n = read(newsockfd, buf, sizeof(buf) - 1);
        if (n < 0) {
            perror("read failed");
            close(newsockfd);
            continue;
        }

        buf[n] = '\0';
        printf("Message from Client: %s\n", buf);
        n = write(newsockfd, buf, strlen(buf));
        if (n < 0) {
            perror("write failed");
        }
        close(newsockfd);
    }
    close(sockfd);
    return 0;
}

