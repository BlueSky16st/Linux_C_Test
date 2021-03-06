#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define RAWPORT 12000

typedef struct tagIpHeader
{
    /*
        网络上使用大字节序，在系统中使用小字节序
        所以需要把ucVer和ucHeadLen的位置调换才是正确值
        同理usFlag和usOffset也需要位置调换
    */
    
    //unsigned char ucVer : 4;
    //unsigned char ucHeadLen : 4;

    unsigned char ucHeadLen : 4;
    unsigned char ucVer : 4;

    unsigned char ucTos;
    unsigned short usLen;
    unsigned short usIdent;
    
    //unsigned short usFlag : 3;
    //unsigned short usOffset : 13;

    unsigned short usOffset : 13;
    unsigned short usFlag : 3;

    unsigned char ucTTL;
    unsigned char ucProtocol;

    unsigned short usChkSum;

    unsigned int uiSrcIp;
    unsigned int uiDstIp;

    char data[0];


} IPHEAD_S;

typedef struct tagTcpHeader
{
    unsigned short srcPort;
    unsigned short dstProt;
    unsigned int uiSeq;
    unsigned int uiSeqQ;
    unsigned short fin:1;
    unsigned short syn:1;
    unsigned short rst:1;
    unsigned short psh:1;
    unsigned short ack:1;
    unsigned short urg:1;
    unsigned short res:6;
    unsigned short usHeadLen:4;
    unsigned short usWinLen;
    unsigned short usChkSum;
    unsigned short usUrgPtr;
    unsigned char data[0];

} TCPHEAD_S;



void CaptureData();
void printIpHead(IPHEAD_S * pIpHead);


int main(void)
{
    CaptureData();

    return 0;
}



// 输出IP头信息
void printIpHead(IPHEAD_S * pIpHead)
{
    printf("\n--------------------\n");
    printf("IP头信息\n");

    printf("版本号：%d\n", pIpHead->ucVer);
    printf("首部长度：%d\n", pIpHead->ucHeadLen);
    printf("服务类型：%d\n", pIpHead->ucTos);

    // 把网络序转主机序
    printf("数据长度：%d\n", ntohs(pIpHead->usLen));
    printf("标识位：%d\n", ntohs(pIpHead->usIdent));

    printf("标识：%d\n", pIpHead->usFlag);
    printf("偏移：%d\n", pIpHead->usOffset);
    printf("TTL：%d\n", pIpHead->ucTTL);
    printf("协议：%d\n", pIpHead->ucProtocol);
    printf("首部检验和:%d\n", ntohs(pIpHead->usChkSum));

    struct sockaddr_in addr;
    // s_addr是网络序，所以uiSrcIp和uiDstIp均不需要转换成主机序
    addr.sin_addr.s_addr = pIpHead->uiSrcIp;
    printf("源地址：%s\n", inet_ntoa(addr.sin_addr));

    addr.sin_addr.s_addr = pIpHead->uiDstIp;
    printf("目标地址：%s\n", inet_ntoa(addr.sin_addr));

    printf("\n--------------------\n");

}

// 输出TCP头信息
void printfTcpHead(TCPHEAD_S * pHead)
{
    printf("\n--------------------\n");
    printf("TCP头信息\n");

    // 把网络序转主机序
    printf("源端口：%d\n", ntohs(pHead->srcPort));
    printf("目标端口：%d\n", ntohs(pHead->dstProt));
    printf("序列号：%d\n", ntohl(pHead->uiSeq));
    printf("确定序列号：%d\n", ntohl(pHead->uiSeqQ));
    printf("标识位：%d %d %d %d %d %d\n", 
           pHead->fin, pHead->syn, pHead->rst,
           pHead->psh, pHead->ack, pHead->urg);
    printf("头部长度：%d\n", pHead->usHeadLen);
    printf("窗口大小：%d\n", ntohs(pHead->usWinLen));
    printf("校验和：%d\n", ntohs(pHead->usChkSum));
    printf("紧急数据偏移量：%d\n", ntohs(pHead->usUrgPtr));

    printf("\n--------------------\n");

}


void CaptureData()
{
    int fd, ret;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    // IPPROTO_TCP：抓TCP包
    fd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    
    // IPPROTO_ICMP：抓ping包
    //fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);

    if(fd < 0)
    {
        perror("Error");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(RAWPORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(fd, (struct sockaddr *)&addr, addrlen);
    if(ret)
    {
        perror("Error");
        close(fd);
        return;
    }

    char Buf[1000];
    while(1)
    {
        memset(Buf, 0, 1000);
        ret = read(fd, Buf, 1000);
        if(ret < 0)
        {
            perror("Error");
            break;
        }

        //printf("Recv: %s\n", Buf);

        IPHEAD_S * pIpHead = (IPHEAD_S *)Buf;
        printIpHead(pIpHead);

        //printf("Data: %s\n", pIpHead->data);

        TCPHEAD_S * pTcpHead = (TCPHEAD_S *)pIpHead->data;
        printfTcpHead(pTcpHead);
        printf("Data: %s\n", pTcpHead->data);



    }
    close(fd);

}
