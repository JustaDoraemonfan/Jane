#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>

struct sockaddr_in serv_addr, cli_addr, active_addr;
socklen_t cli_len;

unsigned short serv_port = 25020;
char serv_ip[] = "127.0.0.1";

char rbuff[512];
char sbuff[512];

int precedence(char op)
{
    if(op=='+' || op=='-') return 1;
    if(op=='*' || op=='/') return 2;
    return 0;
}

int applyOp(int a, int b, char op, int *ok)
{
    if(op=='+') return a+b;
    if(op=='-') return a-b;
    if(op=='*') return a*b;
    if(op=='/')
    {
        if(b==0){ *ok=0; return 0; }
        return a/b;
    }
    *ok=0;
    return 0;
}

int infixToPostfix(const char *infix, char *postfix)
{
    char ops[512];
    int top = -1;
    int k = 0;
    int i = 0;

    while(infix[i] != '\0')
    {
        if(isspace((unsigned char)infix[i])) { i++; continue; }

        if(isdigit((unsigned char)infix[i]))
        {
            while(isdigit((unsigned char)infix[i]))
            {
                postfix[k++] = infix[i++];
            }
            postfix[k++] = ' ';
            continue;
        }

        if(infix[i] == '(')
        {
            ops[++top] = infix[i++];
            continue;
        }

        if(infix[i] == ')')
        {
            while(top >= 0 && ops[top] != '(')
            {
                postfix[k++] = ops[top--];
                postfix[k++] = ' ';
            }
            if(top < 0) return 0;
            top--;
            i++;
            continue;
        }

        if(infix[i]=='+' || infix[i]=='-' || infix[i]=='*' || infix[i]=='/')
        {
            char op = infix[i++];
            while(top >= 0 && ops[top] != '(' && precedence(ops[top]) >= precedence(op))
            {
                postfix[k++] = ops[top--];
                postfix[k++] = ' ';
            }
            ops[++top] = op;
            continue;
        }

        return 0;
    }

    while(top >= 0)
    {
        if(ops[top] == '(') return 0;
        postfix[k++] = ops[top--];
        postfix[k++] = ' ';
    }

    postfix[k] = '\0';
    return 1;
}

int evalPostfix(const char *postfix, int *ok)
{
    int st[512];
    int top = -1;
    int i = 0;

    *ok = 1;

    while(postfix[i] != '\0')
    {
        while(isspace((unsigned char)postfix[i])) i++;
        if(postfix[i] == '\0') break;

        if(isdigit((unsigned char)postfix[i]))
        {
            int val = 0;
            while(isdigit((unsigned char)postfix[i]))
            {
                val = val*10 + (postfix[i]-'0');
                i++;
            }
            st[++top] = val;
            continue;
        }

        if(postfix[i]=='+' || postfix[i]=='-' || postfix[i]=='*' || postfix[i]=='/')
        {
            if(top < 1){ *ok = 0; return 0; }
            int b = st[top--];
            int a = st[top--];
            int res = applyOp(a, b, postfix[i], ok);
            if(!(*ok)) return 0;
            st[++top] = res;
            i++;
            continue;
        }

        *ok = 0;
        return 0;
    }

    if(top != 0){ *ok = 0; return 0; }
    return st[top];
}

int same_client(struct sockaddr_in *a, struct sockaddr_in *b)
{
    return a->sin_addr.s_addr == b->sin_addr.s_addr && a->sin_port == b->sin_port;
}

int main()
{
    int sockfd;
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

        char postfix[512];
        int ok;
        int result;

        if(!infixToPostfix(rbuff, postfix))
        {
            strcpy(sbuff, "Error: invalid expression\n");
        }
        else
        {
            result = evalPostfix(postfix, &ok);
            if(!ok) strcpy(sbuff, "Error: evaluation failed\n");
            else sprintf(sbuff, "Result = %d\n", result);
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
