#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

struct sockaddr_in serv_addr, cli_addr, active_addr;
socklen_t cli_len;

unsigned short serv_port = 25020;
char serv_ip[] = "127.0.0.1";

char rbuff[256];
char sbuff[256];

int same_client(struct sockaddr_in *a, struct sockaddr_in *b)
{
    return a->sin_addr.s_addr == b->sin_addr.s_addr && a->sin_port == b->sin_port;
}

int main()
{
    int sockfd, r, w;
    int active = 0;

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, &serv_addr.sin_addr);

    printf("\nUDP CALCULATOR SERVER.\n");

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\nSERVER ERROR: Cannot create socket.\n");
        exit(1);
    }

    if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nSERVER ERROR: Cannot bind.\n");
        close(sockfd);
        exit(1);
    }

    cli_len = sizeof(cli_addr);

    while(1)
    {
        printf("\nSERVER: Waiting for client...\n");
        r = recvfrom(sockfd, rbuff, sizeof(rbuff)-1, 0, (struct sockaddr*)&cli_addr, &cli_len);
        if(r < 0) continue;

        rbuff[r] = '\0';

        if(!active)
        {
            active_addr = cli_addr;
            active = 1;
            printf("\nSERVER: Active client %s:%d\n", inet_ntoa(active_addr.sin_addr), ntohs(active_addr.sin_port));
        }

        if(!same_client(&cli_addr, &active_addr))
        {
            strcpy(sbuff, "Server busy\n");
            sendto(sockfd, sbuff, strlen(sbuff), 0, (struct sockaddr*)&cli_addr, cli_len);
            continue;
        }

        if(strncmp(rbuff, "exit", 4) == 0)
        {
            strcpy(sbuff, "exit");
            sendto(sockfd, sbuff, strlen(sbuff), 0, (struct sockaddr*)&active_addr, sizeof(active_addr));
            active = 0;
            continue;
        }

        int a, b, result;
        char op;

        if(sscanf(rbuff, "%d %c %d", &a, &op, &b) == 3)
        {
            int ok = 1;

            switch(op)
            {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/':
                    if(b != 0) result = a / b;
                    else ok = 0;
                    break;
                default:
                    ok = 0;
            }

            if(ok) sprintf(sbuff, "Result = %d\n", result);
            else strcpy(sbuff, "Error\n");
        }
        else
        {
            strcpy(sbuff, "Error\n");
        }

        sendto(sockfd, sbuff, strlen(sbuff), 0, (struct sockaddr*)&active_addr, sizeof(active_addr));

        printf("SERVER: Type message to send (or exit): ");
        fflush(stdout);

        if(fgets(sbuff, sizeof(sbuff), stdin) == NULL) continue;

        if(strncmp(sbuff, "exit", 4) == 0)
        {
            strcpy(sbuff, "exit");
            sendto(sockfd, sbuff, strlen(sbuff), 0, (struct sockaddr*)&active_addr, sizeof(active_addr));
            active = 0;
            continue;
        }

        sendto(sockfd, sbuff, strlen(sbuff), 0, (struct sockaddr*)&active_addr, sizeof(active_addr));
    }
}
