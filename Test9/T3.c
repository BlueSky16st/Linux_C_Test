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

} TCPHEADER_S;



void CaptureData();
void printIpHead(IPHEAD_S * pIpHead);


int main(void)
{
    CaptureData();

    return 0;
}



void printIpHead(IPHEAD_S * pIpHead)
{
    printf("\n--------------------\n");

    printf("版本号：%d\n", pIpHead->ucVer);
    printf("首部长度：%d\n", pIpHead->ucHeadLen);
    printf("服务类型：%d\n", pIpHead->ucTos);

    // 把网络序转主机序
    printf("数据长度：%d\n", htons(pIpHead->usLen));
    printf("标识位：%d\n", htons(pIpHead->usIdent));

    printf("标识：%d\n", pIpHead->usFlag);
    printf("偏移：%d\n", pIpHead->usOffset);
    printf("TTL：%d\n", pIpHead->ucTTL);
    printf("协议：%d\n", pIpHead->ucProtocol);
    printf("首部检验和:%d\n", htons(pIpHead->usChkSum));

    struct sockaddr_in addr;
    // s_addr是网络序，所以uiSrcIp和uiDstIp均不需要转换成主机序
    addr.sin_addr.s_addr = pIpHead->uiSrcIp;
    printf("源地址：%s\n", inet_ntoa(addr.sin_addr));

    addr.sin_addr.s_addr = pIpHead->uiDstIp;
    printf("目标地址：%s\n", inet_ntoa(addr.sin_addr));

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
    addr.sin_port = htonl(RAWPORT);
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

        TCPHEADER_S * pTcpHead = (TCPHEADER_S *)pIpHead->data;
        printf("%s\n", pTcpHead->data);



    }
    close(fd);

}
