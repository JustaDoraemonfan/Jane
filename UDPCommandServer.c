#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

static int addr_eq(struct sockaddr_in *a, struct sockaddr_in *b){
    return a->sin_family == b->sin_family && a->sin_port == b->sin_port && a->sin_addr.s_addr == b->sin_addr.s_addr;
}

static int wait_ack(int sockfd, struct sockaddr_in *cli, int seq){
    fd_set rfds;
    struct timeval tv;
    char abuf[128];
    struct sockaddr_in from;
    socklen_t flen = sizeof(from);

    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    int rv = select(sockfd + 1, &rfds, NULL, NULL, &tv);
    if(rv <= 0) return 0;

    int r = recvfrom(sockfd, abuf, sizeof(abuf) - 1, 0, (struct sockaddr*)&from, &flen);
    if(r <= 0) return 0;
    abuf[r] = '\0';

    if(!addr_eq(&from, cli)) return 0;

    int aseq = -1;
    if(sscanf(abuf, "ACK %d", &aseq) == 1 && aseq == seq) return 1;
    return 0;
}

static int send_data_stopwait(int sockfd, struct sockaddr_in *cli, const char *data, long size){
    const int PAY = 900;
    char pkt[1200];
    long off = 0;
    int seq = 0;
    socklen_t clen = sizeof(*cli);

    while(off < size){
        int chunk = (int)((size - off) > PAY ? PAY : (size - off));
        int hdr = snprintf(pkt, sizeof(pkt), "DATA %d %d\n", seq, chunk);
        if(hdr <= 0 || hdr + chunk > (int)sizeof(pkt)) return -1;
        memcpy(pkt + hdr, data + off, chunk);

        int tries = 0;
        while(tries < 10){
            sendto(sockfd, pkt, hdr + chunk, 0, (struct sockaddr*)cli, clen);
            if(wait_ack(sockfd, cli, seq)) break;
            tries++;
        }
        if(tries >= 10) return -1;

        off += chunk;
        seq++;
    }
    return 0;
}

int main(){
    struct sockaddr_in serv_addr, cli_addr, active_cli;
    int sockfd;
    socklen_t cli_len;
    unsigned short serv_port = 25035;
    char serv_ip[] = "127.0.0.1";

    char buff[256];
    char response[65536];

    int has_client = 0;

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, &serv_addr.sin_addr);

    printf("\nUDP COMMAND SERVER\n");

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){ printf("\nSERVER ERROR: Cannot create socket.\n"); exit(1); }

    if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nSERVER ERROR: Cannot bind.\n");
        close(sockfd);
        exit(1);
    }

    while(1){
        if(!has_client) printf("\nSERVER: Waiting for client...\n");

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(0, &readfds);

        int maxfd = sockfd > 0 ? sockfd : 0;
        if(select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) continue;

        if(FD_ISSET(0, &readfds)){
            if(fgets(buff, sizeof(buff), stdin) == NULL) continue;
            if(strncmp(buff, "exit", 4) == 0){
                if(has_client){
                    sendto(sockfd, "exit", 4, 0, (struct sockaddr*)&active_cli, sizeof(active_cli));
                    printf("SERVER: Client disconnected.\n");
                    has_client = 0;
                }
                continue;
            }
            printf("SERVER: No client. Type commands after a client connects.\n");
        }

        if(FD_ISSET(sockfd, &readfds)){
            cli_len = sizeof(cli_addr);
            int r = recvfrom(sockfd, buff, sizeof(buff) - 1, 0, (struct sockaddr*)&cli_addr, &cli_len);
            if(r <= 0) continue;
            buff[r] = '\0';

            if(!has_client){
                active_cli = cli_addr;
                has_client = 1;
                printf("\nConnected to %s:%d\n", inet_ntoa(active_cli.sin_addr), ntohs(active_cli.sin_port));
            }else{
                if(!addr_eq(&cli_addr, &active_cli)){
                    sendto(sockfd, "BUSY", 4, 0, (struct sockaddr*)&cli_addr, cli_len);
                    continue;
                }
            }

            if(strncmp(buff, "exit", 4) == 0){
                printf("Client exited.\n");
                has_client = 0;
                continue;
            }

            printf("CLIENT CMD: %s", buff);

            if(strncmp(buff, "cd", 2) == 0){
                char path[256];
                if(sscanf(buff, "cd %255[^\n]", path) != 1){
                    strcpy(response, "Invalid format");
                }else{
                    if(chdir(path) == 0) strcpy(response, "Directory changed");
                    else strcpy(response, "Failed to change directory");
                }

                char header[64];
                long sz = (long)strlen(response);
                snprintf(header, sizeof(header), "TEXT %ld", sz);
                sendto(sockfd, header, strlen(header), 0, (struct sockaddr*)&active_cli, sizeof(active_cli));
                send_data_stopwait(sockfd, &active_cli, response, sz);
                continue;
            }

            FILE *fp = popen(buff, "r");
            if(fp == NULL){
                strcpy(response, "Error executing command");
            }else{
                response[0] = '\0';
                char temp[512];
                size_t used = 0;
                while(fgets(temp, sizeof(temp), fp) != NULL){
                    size_t t = strlen(temp);
                    if(used + t + 1 >= sizeof(response)) break;
                    memcpy(response + used, temp, t);
                    used += t;
                    response[used] = '\0';
                }
                pclose(fp);
            }

            long outsz = (long)strlen(response);
            if(outsz == 0){
                strcpy(response, "\n");
                outsz = 1;
            }

            char header[64];
            snprintf(header, sizeof(header), "TEXT %ld", outsz);
            sendto(sockfd, header, strlen(header), 0, (struct sockaddr*)&active_cli, sizeof(active_cli));

            if(send_data_stopwait(sockfd, &active_cli, response, outsz) != 0){
                printf("SERVER ERROR: Transfer failed.\n");
                has_client = 0;
            }
        }
    }

    close(sockfd);
    return 0;
}
