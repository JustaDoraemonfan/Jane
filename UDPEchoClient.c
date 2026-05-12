#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct sockaddr_in serv_addr;

int skfd, r, w, serv_addr_len;

unsigned short serv_port = 25020;
char serv_ip[] = "127.0.0.1";

char rbuff[128];
char sbuff[128] = "===good morning===";

int main()
{
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, (&serv_addr.sin_addr));

    printf("\nUDP ECHO CLIENT.\n");

    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\nCLIENT ERROR: Cannot create socket.\n");
        exit(1);
    }

    serv_addr_len = sizeof(serv_addr);

    w = sendto(skfd, sbuff, 128, 0, (struct sockaddr *)&serv_addr, serv_addr_len);
    if (w < 0)
    {
        printf("\nCLIENT ERROR: Cannot send message to the server.\n");
        close(skfd);
        exit(1);
    }
    printf("\nCLIENT: Message sent to echo server.\n");

    r = recvfrom(skfd, rbuff, 128, 0, NULL, NULL);
    if (r < 0)
        printf("\nCLIENT ERROR: Cannot receive message from server.\n");
    else
    {
        rbuff[r] = '\0';
        printf("\nCLIENT: Message from echo server: %s\n", rbuff);
    }

    close(skfd);
    return 0;
}
