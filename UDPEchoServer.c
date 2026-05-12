#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct sockaddr_in serv_addr, cli_addr;

int skfd, r, w, cli_addr_len;

unsigned short serv_port = 25020;
char serv_ip[] = "127.0.0.1";

char buff[128];

int main()
{
    bzero(&serv_addr, sizeof(serv_addr));
    bzero(&cli_addr, sizeof(cli_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, (&serv_addr.sin_addr));

    printf("\nUDP ECHO SERVER.\n");

    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\nSERVER ERROR: Cannot create socket.\n");
        exit(1);
    }

    if ((bind(skfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("\nSERVER ERROR: Cannot bind.\n");
        close(skfd);
        exit(1);
    }

    cli_addr_len = sizeof(cli_addr);

    for (;;)
    {
        printf("\nSERVER: Waiting for messages... Press Ctrl + C to stop:\n");

        r = recvfrom(skfd, buff, 128, 0, (struct sockaddr *)&cli_addr, (socklen_t *)&cli_addr_len);
        if (r < 0)
        {
            printf("\nSERVER ERROR: Cannot receive message.\n");
            continue;
        }

        buff[r] = '\0';
        printf("\nSERVER: Received '%s' from %s:%d\n",
               buff, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        w = sendto(skfd, buff, 128, 0, (struct sockaddr *)&cli_addr, cli_addr_len);
        if (w < 0)
            printf("\nSERVER ERROR: Cannot send echo.\n");
        else
            printf("\nSERVER: Echoed back to %s:%d\n",
                   inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
    }

    close(skfd);
    return 0;
}
