#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct sockaddr_in serv_addr;
socklen_t serv_len;

int skfd, r, w;

unsigned short serv_port = 25020;
char serv_ip[] = "127.0.0.1";

char sbuff[128] = "time";
char rbuff[128];

int main()
{
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);

    if (inet_aton(serv_ip, &serv_addr.sin_addr) == 0)
    {
        printf("\nCLIENT ERROR: Invalid server IP.\n");
        exit(1);
    }

    printf("\nUDP TIME CLIENT.\n");

    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\nCLIENT ERROR: Cannot create socket.\n");
        exit(1);
    }

    serv_len = sizeof(serv_addr);

    for (;;)
    {
        w = sendto(skfd, sbuff, strlen(sbuff), 0, (struct sockaddr *)&serv_addr, serv_len);
        if (w < 0)
        {
            printf("\nCLIENT ERROR: Cannot send request.\n");
            break;
        }

        r = recvfrom(skfd, rbuff, 127, 0, NULL, NULL);
        if (r < 0)
        {
            printf("\nCLIENT ERROR: Cannot receive.\n");
            break;
        }

        rbuff[r] = '\0';

        if (strcmp(rbuff, "exit") == 0)
        {
            printf("\nSERVER: exit received. Client disconnected.\n");
            break;
        }

        printf("\nSERVER TIME: %s", rbuff);
        sleep(2);
    }

    close(skfd);
    return 0;
}
