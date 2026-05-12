#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int sockfd, r;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    unsigned short serv_port = 25020;
    char serv_ip[] = "127.0.0.1";
    char buff[128];

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, &serv_addr.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("UDP CHAT SERVER STARTED\n");

    while (1) {
        printf("\nWaiting for client message...\n");

        cli_len = sizeof(cli_addr);
        r = recvfrom(sockfd, buff, sizeof(buff) - 1, 0,
                     (struct sockaddr*)&cli_addr, &cli_len);

        if (r < 0) { perror("recvfrom"); continue; }

        buff[r] = '\0';

        printf("Client (%s:%d): %s",
               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buff);

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client exited. Waiting for new client...\n");
            continue;
        }

        while (1) {
            printf("Server: ");
            if (fgets(buff, sizeof(buff), stdin) == NULL) continue;

            sendto(sockfd, buff, strlen(buff), 0,
                   (struct sockaddr*)&cli_addr, cli_len);

            if (strncmp(buff, "exit", 4) == 0) {
                printf("Client disconnected. Waiting for new client...\n");
                break;
            }

            cli_len = sizeof(cli_addr);
            r = recvfrom(sockfd, buff, sizeof(buff) - 1, 0,
                         (struct sockaddr*)&cli_addr, &cli_len);

            if (r < 0) { perror("recvfrom"); break; }

            buff[r] = '\0';

            printf("Client (%s:%d): %s",
                   inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buff);

            if (strncmp(buff, "exit", 4) == 0) {
                printf("Client exited. Waiting for new client...\n");
                break;
            }
        }
    }

    close(sockfd);
    return 0;
}
