#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>

struct sockaddr_in serv_addr, cli_addr, active_cli;

int skfd, r, w;
socklen_t cli_addr_len;

unsigned short serv_port = 25020;
char serv_ip[] = "127.0.0.1";

char buff[128];
char cmd[128];

int same_client(struct sockaddr_in *a, struct sockaddr_in *b)
{
    return a->sin_addr.s_addr == b->sin_addr.s_addr && a->sin_port == b->sin_port;
}

int main()
{
    int have_client = 0;

    bzero(&serv_addr, sizeof(serv_addr));
    bzero(&cli_addr, sizeof(cli_addr));
    bzero(&active_cli, sizeof(active_cli));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);

    if (inet_aton(serv_ip, &serv_addr.sin_addr) == 0)
    {
        printf("\nSERVER ERROR: Invalid IP.\n");
        exit(1);
    }

    printf("\nUDP TIME SERVER.\n");

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
        fd_set rfds;
        int maxfd;

        FD_ZERO(&rfds);
        FD_SET(skfd, &rfds);
        FD_SET(STDIN_FILENO, &rfds);

        maxfd = (skfd > STDIN_FILENO) ? skfd : STDIN_FILENO;

        if (select(maxfd + 1, &rfds, NULL, NULL, NULL) < 0)
        {
            printf("\nSERVER ERROR: select failed.\n");
            continue;
        }

        if (FD_ISSET(STDIN_FILENO, &rfds))
        {
            if (fgets(cmd, sizeof(cmd), stdin) != NULL)
            {
                int n = strlen(cmd);
                if (n > 0 && cmd[n - 1] == '\n') cmd[n - 1] = '\0';

                if (strcmp(cmd, "exit") == 0)
                {
                    if (have_client)
                    {
                        sendto(skfd, "exit", 4, 0, (struct sockaddr *)&active_cli, sizeof(active_cli));
                        printf("\nSERVER: Sent exit to %s:%d\n",
                               inet_ntoa(active_cli.sin_addr), ntohs(active_cli.sin_port));
                        have_client = 0;
                    }
                    else
                    {
                        printf("\nSERVER: No active client.\n");
                    }
                }
            }
        }

        if (FD_ISSET(skfd, &rfds))
        {
            r = recvfrom(skfd, buff, 127, 0, (struct sockaddr *)&cli_addr, &cli_addr_len);
            if (r < 0)
            {
                printf("\nSERVER ERROR: Cannot receive message.\n");
                continue;
            }

            buff[r] = '\0';

            if (!have_client)
            {
                active_cli = cli_addr;
                have_client = 1;
                printf("\nSERVER: Active client is %s:%d\n",
                       inet_ntoa(active_cli.sin_addr), ntohs(active_cli.sin_port));
            }

            if (!same_client(&cli_addr, &active_cli))
            {
                continue;
            }

            if (strcmp(buff, "exit") == 0)
            {
                printf("\nSERVER: Client exit received from %s:%d\n",
                       inet_ntoa(active_cli.sin_addr), ntohs(active_cli.sin_port));
                have_client = 0;
                continue;
            }

            time_t t = time(NULL);
            char *ts = ctime(&t);
            if (!ts) ts = "time error\n";

            w = sendto(skfd, ts, strlen(ts), 0, (struct sockaddr *)&active_cli, sizeof(active_cli));
            if (w < 0)
                printf("\nSERVER ERROR: Cannot send time.\n");
            else
                printf("\nSERVER: Time sent to %s:%d\n",
                       inet_ntoa(active_cli.sin_addr), ntohs(active_cli.sin_port));
        }
    }

    close(skfd);
    return 0;
}
