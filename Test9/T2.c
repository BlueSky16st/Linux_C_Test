#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERV_PORT 0xabcd
#define CLIENT_PORT 0xabce

void UDP_Server();
void UDP_Client();

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        printf("s:UDP服务器\n");
        printf("c:UDP客户端\n");
        return 0;
    }

    if(argv[1][0] == 's')
        UDP_Server();
    else if(argv[1][0] == 'c')
        UDP_Client();

    return 0;
}

void UDP_Server()
{
    int fd, ret;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        perror("Error");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERV_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(fd, (struct sockaddr *)&addr, addrlen);
    if(ret)
    {
        perror("Error");
        close(fd);
        return;
    }

    struct sockaddr_in srcaddr;
    char szBuf[1024];
    char szMsg[] = "Receive";
    while(1)
    {
        memset(szBuf, 0, 1024);

        ret = recvfrom(fd, szBuf, 1024, 0, (struct sockaddr *)&srcaddr, &addrlen);
        if(ret < 0)
        {
            perror("Error");
            break;
        }

        printf("Recv: %s\n", szBuf);

        sendto(fd, szMsg, strlen(szMsg), 0, (struct sockaddr *)&srcaddr, addrlen);

    }

    close(fd);

}


void UDP_Client()
{
    int fd, ret;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        perror("Error");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(CLIENT_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(fd, (struct sockaddr *)&addr, addrlen);
    if(ret)
    {
        perror("Error");
        close(fd);
        return;
    }

    struct sockaddr_in srvaddr;
    char szIp[16] = "";
    int port;

    fprintf(stderr, "输入目标IP地址和端口号：");
    scanf("%s%d", szIp, &port);

    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons((unsigned short)port);
    srvaddr.sin_addr.s_addr = inet_addr(szIp);

    char szBuf[1024], szRecv[1024];
    while(1)
    {
        memset(szBuf, 0, 1024);

        fprintf(stderr, "->");
        read(STDIN_FILENO, szBuf, 1024);

        sendto(fd, szBuf, strlen(szBuf), 0, (struct sockaddr *)&srvaddr, addrlen);
        
        memset(szRecv, 0, 1024);
        ret = recvfrom(fd, szRecv, 1024, 0, (struct sockaddr *)&srvaddr, &addrlen);
        if(ret < 0)
        {
            perror("Error");
            break;
        }

        printf("Recv: %s\n", szRecv);
    }
    close(fd);


}