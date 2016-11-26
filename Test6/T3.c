#include <stdio.h>
#include <sys/time.h>
#include <sys/select.h>

void TestSelect();

int main(void)
{
    TestPipe();

    return 0;
}


// 监控多个文件描述符的输出和输出
void TestSelect()
{
    fd_set fset;

    char szBuf[100];
    struct timeval tv;
    while(1)
    {
        // 清空fdset
        FD_ZERO(&fset);
        // 监控stdin
        FD_SET(0, &fset);
        
        tv.tv_sec = 3;
        tv.tv_usec = 0;

        fprintf(stderr, "Input:");
        int iRet = select(1, &fset, NULL, NULL, &tv);

        if(iRet == -1)
        {
            perror("select error");
        }
        else if(iRet == 0)
        {
            printf("No input\n");
        }
        else
        {
            if(FD_ISSET(0, &fset))
            {
                scanf("%s", szBuf);
                printf("%s\n", szBuf);
            }
        }
    }

}
