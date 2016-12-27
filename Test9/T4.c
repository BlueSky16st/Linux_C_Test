#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SRV_PROT 0xabcd
#define MAX_CONN 1000

void TcpServer();
void RecvMsg(int * fdArr, int index, int * pnConn);

int main(void)
{
    TcpServer();

    return 0;
}

// 使用select模型
void TcpServer()
{
    int fd, ret;
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("Error");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SRV_PROT);

    ret = bind(fd, (struct sockaddr *)&addr, addrLen);
    if(ret)
    {
        perror("Error");
        close(fd);
        return;
    }

    ret = listen(fd, 100);
    if(ret)
    {
        perror("Error");
        close(fd);
        return;
    }



    fd_set fdset;
    int maxfd = fd;
    int fdArr[MAX_CONN];    // 保存所有连接的fd
    int nConn = 0;          // 连接数
    int clientFd;
    struct sockaddr_in srcAddr;
    char szMsg[100];

    while(1)
    {
        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);

        for(int i = 0; i < nConn; i++)
        {
            FD_SET(fdArr[i], &fdset);
        }



        // 发送消息给所有客户端
        fprintf(stderr, "Send: ");
        scanf("%s", szMsg);
        for(int i = 0; i < nConn; i++)
        {
            write(fdArr[i], szMsg, strlen(szMsg));
        }



        ret = select(maxfd + 1, &fdset, NULL, NULL, NULL);

        if(ret < 0)
        {
            perror("Error");
            break;
        }
        else if(ret == 0)
        {
            // 超时

        }
        else
        {
            if(FD_ISSET(fd, &fdset))
            {
                clientFd = accept(fd, (struct sockaddr *)&srcAddr, &addrLen);
                if(clientFd < 0)
                {
                    perror("Error");
                    break;
                }
                
                if(nConn == MAX_CONN)
                {
                    char szTip[] = "已达最大连接数";
                    write(clientFd, szTip, sizeof(szTip));
                    printf("已达最大连接数，新的连接已断开\n");
                    close(clientFd);
                }
                else
                {
                    char szTip[] = "Welcome!";
                    write(clientFd, szTip, sizeof(szTip));
                    
                    printf("有新连接，来自：[%d](%s:%d)\n", clientFd, 
                                                         inet_ntoa(srcAddr.sin_addr), 
                                                         ntohs(srcAddr.sin_port));

                    fdArr[nConn++] = clientFd;
                    if(clientFd > maxfd)
                    {
                        maxfd = clientFd;
                    }

                }

            }

            for(int i = 0; i < nConn; i++)
            {
                if(FD_ISSET(fdArr[i], &fdset))
                {
                    RecvMsg(fdArr, i, &nConn);
                }
            }

        }

    }

    close(fd);

}

void RecvMsg(int * fdArr, int index, int * pnConn)
{
    char szBuf[1024] = { 0 };
    int ret;

    ret = read(fdArr[index], szBuf, 1024);
    if(ret < 0)
    {
        perror("Error");
        return;
    }
    else if(ret == 0)
    {
        // 断开连接
        // 需要从fdArr中删除fd
        printf("[%d]已断开连接\n", fdArr[index]);
        close(fdArr[index]);
        for(int i = index + 1; i < *pnConn; i++)
        {
            fdArr[index] = fdArr[i];
        }
        (*pnConn)--;
    }
    else
    {
        printf("接收：%s\n", szBuf);
    }


}

