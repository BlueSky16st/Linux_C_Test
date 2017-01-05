#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERV_PORT 0xabcd        // 端口号

void TCP_Server();
void TCP_Client();
void * Comm_thread(void * param);

int main(int argc, char ** argv)
{
    if(argc != 2 || (strcmp(argv[1], "c") && strcmp(argv[1], "s")))
    {
        printf("c：启用客户端\ns：启用服务器\n");
        return 0;
    }

    if(argv[1][0] == 's')
        TCP_Server();
    else
        TCP_Client();

    return 0;
}

// 创建服务端
void TCP_Server()
{
    int fd;
    
    // PF_INET：Ipv4
    fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("socket Fail");
        return;
    }

    // AF_INET：Ipv4
    struct sockaddr_in addr;
    int ret;
    socklen_t addrLen = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERV_PORT);           // 设置端口号
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   // 接收任何ip地址

    ret = bind(fd, (struct sockaddr *)&addr, addrLen);
    if(ret)
    {
        perror("bind Fail");
        close(fd);
        return;
    }


    ret = listen(fd, 10);
    if(ret)
    {
        perror("listen Fail");
        close(fd);
        return;
    }


    int clientFd;
    struct sockaddr_in clientAddr;
    char szTip[] = "Hello";
    char szBuf[1024] = { 0 };

    while(1)
    {
        clientFd = accept(fd, (struct sockaddr *)&clientAddr, &addrLen);
        if(clientFd < 0)
        {
            perror("accept Fail");
            break;
        }

        // inet_ntoa()：数字型地址转字符串地址
        // ntohs()：网络序转主机序（大字节序转小字节序）
        printf("客户端已连接：%s:%d\n", inet_ntoa(clientAddr.sin_addr), 
                                       ntohs(clientAddr.sin_port));
        
        
        // 发送数据
        //send(clientFd, szTip, strlen(szTip), 0);
        write(clientFd, szTip, strlen(szTip));

        // 使用线程对每个客户端做响应
        pthread_t pt;
        pthread_create(&pt, NULL, Comm_thread, (void *)&clientFd);



        /*
        while(1)
        {        
            // 接收数据
            memset(szBuf, 0, 1024);
            //ret = recv(clientFd, szBuf, 1024, 0);
            ret = read(clientFd, szBuf, 1024);
            if(ret < 0)
            {
                perror("read Fail");
                break;
            }
            if(ret == 0)
            {
                printf("断开连接\n");
                break;
            }
            printf("recv: %s\n", szBuf);
        }

        // 关闭客户端连接
        close(clientFd);
        */


    }

    close(fd);

}

// 创建客户端
void TCP_Client()
{
    char szIP[16];
    int port;

    printf("输入IP地址与端口：");
    scanf("%s%d", szIP, &port);

    int fd, ret;
    struct sockaddr_in servAddr;
    socklen_t addrLen = sizeof(servAddr);

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("socket Fail");
        return;
    }

    // 连接服务端
    // inet_addr()：字符串IP地址转换为数字型IP地址
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons((short)port);
    servAddr.sin_addr.s_addr = inet_addr(szIP);
    ret = connect(fd, (struct sockaddr *)&servAddr, addrLen);
    if(ret)
    {
        perror("connect Fail");
        return;
    }

    char szBuf[1024], szMsg[100];

    while(1)
    {
        memset(szBuf, 0, 1024);
        ret = read(fd, szBuf, 1024);
        if(ret < 0)
        {
            perror("read Fail");
            break;
        }
        if(ret == 0)
        {
            printf("服务器断开连接\n");
            break;
        }
        printf("recv: %s\n", szBuf);



        fprintf(stderr, "send: ");

        memset(szBuf, 0, 1024);
        read(0, szMsg, 100);
        ret = write(fd, szBuf, strlen(szMsg));
        
    }

}

void * Comm_thread(void * param)
{
    int fd = *(int *)param;
    int ret;

    char szBuf[1024];
    char szMsg[] = "I received.";

    while(1)
    {
        memset(szBuf, 0, 1024);
        ret = read(fd, szBuf, 1024);

        if(ret < 0)
        {
            perror("read Fail");
            break;
        }
        if(ret == 0)
        {
            printf("断开连接\n");
            break;
        }
        
        printf("接收消息：%s\n", szBuf);

        write(fd, szMsg, sizeof(szMsg));

    }
    close(fd);
    
    return NULL;

}




