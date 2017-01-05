#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/epoll.h>

#include <map>

#define SERV_PORT 12345
#define PROCESS_NUM 2   // epoll_wait()同时处理事件的个数
#define CONN_MAX 100    // 连接限制

int fd = 0, nConn = 0;
std::map<int, struct sockaddr_in> g_fdmap;

void EPoll();
void epoll_process(int epfd, struct epoll_event * ev_res, int cnt);

int main(void)
{
    EPoll();
    

    return 0;
}

// epoll模型
void EPoll()
{
    int ret;
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
    addr.sin_port = SERV_PORT;

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



    int epfd = epoll_create(1); // 参数可以是任意大于0的值
    if(epfd < 0)
    {
        perror("Error");
        close(fd);
        return;
    }

    struct epoll_event ev;

    // 监控标准输入
    ev.events = EPOLLIN;  
    ev.data.fd = STDIN_FILENO;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

    // 监控TCP服务
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);



    struct epoll_event ev_res[PROCESS_NUM];
    while(1)
    {
        ret = epoll_wait(epfd, ev_res, PROCESS_NUM, -1);
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
            epoll_process(epfd, ev_res, ret);
        }
    }

    close(fd);

}

void epoll_process(int epfd, struct epoll_event * ev_res, int cnt)
{
    int clientfd, ret;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    struct epoll_event ev;
    char szBuf[100] = { 0 };
    std::map<int, struct sockaddr_in>::iterator it; 

    for(int i = 0; i < cnt; i++)
    {
        if(ev_res[i].data.fd == STDIN_FILENO)
        {
            // 输入消息给所有客户端
        }
        else if(ev_res[i].data.fd == fd)
        {
            // 新的连接

            clientfd = accept(fd, (struct sockaddr*)&addr, &addrlen);
            if(clientfd < 0)
            {
                perror("Error");
                continue;
            }
            if(nConn == CONN_MAX)
            {
                printf("\n超出连接数\n");
                write(clientfd, "超出连接数\n", strlen("超出连接数\n"));
                close(clientfd);
                continue;
            }


            g_fdmap.insert(std::make_pair(clientfd, addr));
            
            // 添加客户端EPOLLIN事件到epoll中
            ev.events = EPOLLIN;
            ev.data.fd = clientfd;
            epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);


            write(clientfd, "Welcome", strlen("Welcome"));
            printf("\n有新的连接[%s:%d]\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));


            nConn++;
        }
        else
        {
            // 客户端消息
            
            memset(szBuf, 0, 100);

            it = g_fdmap.find(ev_res[i].data.fd);
            if(it == g_fdmap.end())
            {
                // 查找失败
                perror("Error");
                continue;
            }

            ret = read(ev_res[i].data.fd, szBuf, 100);
            if(ret < 0)
            {
                perror("Error");
                continue;
            }
            else if(ret == 0)
            {
                printf("\n断开连接[%s:%d]\n", inet_ntoa((it->second).sin_addr), htons((it->second).sin_port));

                epoll_ctl(epfd, EPOLL_CTL_DEL, ev_res[i].data.fd, ev_res + i);
                g_fdmap.erase(it);

                continue;
            }
            else
            {
                printf("\n接收到数据[%s:%d]：%s\n", inet_ntoa((it->second).sin_addr), htons((it->second).sin_port), szBuf);
            }

        }

    }

}