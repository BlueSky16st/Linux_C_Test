#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define SERV_PORT 12345
#define CONN_MAX 100

void PollServer();
void PollProcess(struct pollfd * pfds, int * nfds);


int main(void)
{
    PollServer();

    return 0;
}


// poll模型
void PollServer()
{
    int fd, ret;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("Error");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERV_PORT);

    ret = bind(fd, (struct sockaddr *)&addr, addrlen);
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


    // 初始化pollfd
    struct pollfd * pfds = (struct pollfd *)malloc(CONN_MAX * sizeof(struct pollfd));
    int nfds = 0;   // 当前pfds总个数

    // 监控标准输入
    pfds[0].fd = 0;
    pfds[0].events = POLLIN;    // 监控可读事件
    nfds++;

    // 监控TCP服务端口
    pfds[1].fd = fd;
    pfds[1].events = POLLIN;
    nfds++;



    // 监控UDP服务端口
    int udpfd = socket(PF_INET, SOCK_DGRAM, 0);
    if(udpfd < 0)
    {
        perror("Error");
        
        close(fd);
        free(pfds);

        return;
    }

    ret = bind(udpfd, (struct sockaddr *)&addr, addrlen);
    if(ret)
    {
        perror("Error");

        close(fd);
        close(udpfd);
        free(pfds);

        return;
    }

    pfds[2].fd = udpfd;
    pfds[2].events = POLLIN;
    nfds++;




    while(1)
    {
        fprintf(stderr, "发送：");
        
        // 参数3：超时时间，<0 一直等待，=0 立即返回，>0 等待指定毫秒数
        ret = poll(pfds, nfds, -1);
        if(ret == 0)
        {
            // 超时
            continue;
        }
        else if(ret < 0)
        {
            perror("Error");
            break;
        }
        else
        {
            PollProcess(pfds, &nfds);
        }

    }

    close(fd);
    close(udpfd);
    
    for(int i = 3; i < nfds; i++)
    {
        close(pfds[i].fd);
    }

    free(pfds);

}

void PollProcess(struct pollfd * pfds, int * nfds)
{
    char szMsg[100] = { 0 }, szBuf[100] = { 0 };
    int fd = 0, ret;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    if(pfds[0].revents & POLLIN)    // 判断POLLIN是否为1
    {
        // 此时标准输入可读
        read(0, szMsg, 100);

        for(int i = 3; i < *nfds; i++)
        {
            write(pfds[i].fd, szMsg, strlen(szMsg));
        }

    }

    if(pfds[1].revents & POLLIN)
    {
        // TCP的消息

        fd = accept(pfds[1].fd, (struct sockaddr *)&addr, &addrlen);
        if(fd < 0)
        {
            perror("Error");
            return;
        }
        if(*nfds == CONN_MAX)
        {
            printf("\n已达最大连接数\n");
            write(fd, "已达最大连接数", sizeof("已达最大连接数"));
            close(fd);
            return;
        }
        else
        {
            write(fd, "Welcome", sizeof("Welcome"));
            printf("\n[%d]有新的连接[%s:%d]\n", fd, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

            pfds[*nfds].fd = fd;
            pfds[*nfds].events = POLLIN;
            (*nfds)++;

        }

    }
   
    if(pfds[2].revents & POLLIN)
    {
        // UDP的消息
        ret = recvfrom(pfds[2].fd, szBuf, 100, 0, (struct sockaddr *)&addr, &addrlen);
        if(ret > 0)
        {
            printf("\n[UDP]接收数据[%s:%d]：%s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), szBuf);
        }

    }


    for(int i = 3; i < *nfds; i++)
    {
        if(pfds[i].revents & POLLIN)
        {
            memset(szBuf, 0, 100);
            
            ret = read(pfds[i].fd, szBuf, 100);
            if(ret < 0)
            {
                perror("Error");
                break;
            }
            else if(ret == 0)
            {
                // 断开连接
                printf("\n[%d]断开连接\n", pfds[i].fd);
                close(pfds[i].fd);
                pfds[i].fd = pfds[*nfds - 1].fd;
                (*nfds)--;
                i--;

            }
            else
            {
                printf("\n[%d]接收数据：%s\n", pfds[i].fd, szBuf);
            }
        }
    }
}