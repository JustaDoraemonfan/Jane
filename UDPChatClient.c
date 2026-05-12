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
    struct sockaddr_in serv_addr;
    socklen_t serv_len;
    unsigned short serv_port = 25020;
    char serv_ip[] = "127.0.0.1";
    char buff[128];

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, &serv_addr.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    serv_len = sizeof(serv_addr);

    printf("UDP CHAT CLIENT STARTED\n");

    while (1) {
        printf("Client: ");
        if (fgets(buff, sizeof(buff), stdin) == NULL) continue;

        sendto(sockfd, buff, strlen(buff), 0,
               (struct sockaddr*)&serv_addr, serv_len);

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client exiting...\n");
            break;
        }

        r = recvfrom(sockfd, buff, sizeof(buff) - 1, 0, NULL, NULL);
        if (r < 0) { perror("recvfrom"); break; }

        buff[r] = '\0';
        printf("Server: %s", buff);

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Server ended chat. Client disconnecting...\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
