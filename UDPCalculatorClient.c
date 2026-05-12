#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

int main()
{
    struct sockaddr_in serv_addr;
    socklen_t serv_len;

    int skfd, r, w;

    unsigned short serv_port = 25020;
    char serv_ip[] = "127.0.0.1";

    char rbuff[256];
    char sbuff[256];

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, &serv_addr.sin_addr);

    printf("\nUDP CALCULATOR CLIENT.\n");

    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\nCLIENT ERROR: Cannot create socket.\n");
        exit(1);
    }

    serv_len = sizeof(serv_addr);

    while(1)
    {
        printf("\nEnter expression (a + b) or 'exit': ");
        if(fgets(sbuff, sizeof(sbuff), stdin) == NULL) break;

        w = sendto(skfd, sbuff, strlen(sbuff), 0, (struct sockaddr*)&serv_addr, serv_len);
        if(w < 0)
        {
            printf("\nCLIENT ERROR: Cannot send.\n");
            break;
        }

        r = recvfrom(skfd, rbuff, sizeof(rbuff)-1, 0, (struct sockaddr*)&serv_addr, &serv_len);
        if(r < 0)
        {
            printf("\nCLIENT ERROR: Cannot receive.\n");
            break;
        }

        rbuff[r] = '\0';

        if(strncmp(rbuff, "exit", 4) == 0)
        {
            printf("SERVER: exit\n");
            break;
        }

        printf("SERVER: %s", rbuff);

        r = recvfrom(skfd, rbuff, sizeof(rbuff)-1, 0, (struct sockaddr*)&serv_addr, &serv_len);
        if(r > 0)
        {
            rbuff[r] = '\0';
            if(strncmp(rbuff, "exit", 4) == 0)
            {
                printf("SERVER: exit\n");
                break;
            }
            printf("SERVER MSG: %s", rbuff);
        }

        if(strncmp(sbuff, "exit", 4) == 0) break;
    }

    close(skfd);
}
