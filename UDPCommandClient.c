#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int send_ack(int sockfd, struct sockaddr_in *serv, int seq){
    char abuf[64];
    socklen_t slen = sizeof(*serv);
    int n = snprintf(abuf, sizeof(abuf), "ACK %d", seq);
    if(n <= 0) return -1;
    return sendto(sockfd, abuf, n, 0, (struct sockaddr*)serv, slen);
}

static int recv_exact_text(int sockfd, struct sockaddr_in *serv, long size){
    long got = 0;
    int expected = 0;

    while(got < size){
        char pkt[1400];
        struct sockaddr_in from;
        socklen_t flen = sizeof(from);

        int r = recvfrom(sockfd, pkt, sizeof(pkt), 0, (struct sockaddr*)&from, &flen);
        if(r <= 0) continue;

        if(from.sin_addr.s_addr != serv->sin_addr.s_addr || from.sin_port != serv->sin_port) continue;

        int seq = -1, len = -1;
        char *nl = memchr(pkt, '\n', r);
        if(!nl) continue;

        int hdrlen = (int)(nl - pkt + 1);
        if(sscanf(pkt, "DATA %d %d\n", &seq, &len) != 2) continue;
        if(hdrlen + len > r) continue;
        if(len < 0) continue;

        if(seq == expected){
            fwrite(pkt + hdrlen, 1, len, stdout);
            fflush(stdout);
            got += len;
            send_ack(sockfd, serv, seq);
            expected++;
        }else{
            send_ack(sockfd, serv, expected - 1);
        }
    }
    return 0;
}

int main(){
    int sockfd;
    struct sockaddr_in serv_addr;
    unsigned short serv_port = 25035;
    char serv_ip[] = "127.0.0.1";

    char buff[256];

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, &serv_addr.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){ printf("CLIENT ERROR: Cannot create socket.\n"); exit(1); }

    printf("\nUDP COMMAND CLIENT\n");

    while(1){
        printf("\nEnter command: ");
        if(fgets(buff, sizeof(buff), stdin) == NULL) break;

        sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

        if(strncmp(buff, "exit", 4) == 0){
            printf("CLIENT: Disconnected.\n");
            break;
        }

        char header[128];
        struct sockaddr_in from;
        socklen_t flen = sizeof(from);

        int r = recvfrom(sockfd, header, sizeof(header) - 1, 0, (struct sockaddr*)&from, &flen);
        if(r <= 0) continue;

        header[r] = '\0';

        if(from.sin_addr.s_addr != serv_addr.sin_addr.s_addr || from.sin_port != serv_addr.sin_port) continue;

        if(r == 4 && strncmp(header, "exit", 4) == 0){
            printf("\nSERVER: exit\nCLIENT: Disconnected.\n");
            break;
        }

        if(strncmp(header, "BUSY", 4) == 0){
            printf("SERVER: BUSY\n");
            continue;
        }

        long size = -1;
        if(sscanf(header, "TEXT %ld", &size) != 1 || size < 0){
            printf("SERVER: Invalid response\n");
            continue;
        }

        printf("\nSERVER OUTPUT:\n");
        recv_exact_text(sockfd, &serv_addr, size);
        printf("\n");
    }

    close(sockfd);
    return 0;
}
