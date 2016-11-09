#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

typedef struct tagAAA
{
    int x;
    int y;
    char szBuf[8];
} AAA;

// 将结构体写入文件
void Test_fwrite() 
{
    FILE * pFile;
    AAA arrA[] = 
    { 
        { 1, 2, "Hello" },
        { 3, 4, "world" }
    };

    pFile = fopen("aaa", "w");
    if(pFile == NULL)
    {
        perror("Fail to fopen\n");
        return;
    }
    fwrite(arrA, sizeof(AAA), 2, pFile);
    fclose(pFile);

}

// 读取文件中的结构体
void Test_fread()
{
    FILE * pFile;
    AAA A;

    pFile = fopen("aaa", "r");
    if(pFile == NULL)
    {
        perror("Fail to fopen\n");
        return;
    }

	// 读取第二个
    fseek(pFile, sizeof(AAA), SEEK_SET);

    fread(&A, sizeof(AAA), 1, pFile);
    printf("{ %d, %d, %s }\n", A.x, A.y, A.szBuf);

    rewind(pFile);

    fread(&A, sizeof(AAA), 1, pFile);
    printf("{ %d, %d, %s }\n", A.x, A.y, A.szBuf);

    fclose(pFile);

}

// 测试stdout
// setbuf()可以设置stdout无缓冲区
void TestBuf()
{
    //setbuf(stdout, NULL);
    while(1)
    {
        usleep(100000);
        printf("123456");
        fprintf(stderr, "*");
    }

}

// stdout行缓冲，直到碰到'\n'或缓冲区满时输出
// stderr无缓冲，直接输出
void TestPrint()
{
    printf("1");
    fprintf(stderr, "2");
    fprintf(stdout, "3");
    fprintf(stderr, "4");
    printf("5\n");
    
}

// 可变参数，求和
int sum(int cnt, ...)
{
    int iRet = 0;
    int num = 0;

    va_list args;
    va_start(args, cnt);

    num = (int)va_arg(args, int);
    while(cnt--)
    {
        iRet += num;
        num = (int)va_arg(args, int);
    }
    va_end(args);

    return iRet;

}

// 可变参数，输出到文件
void writeLog(char * fmt, ...)
{
    FILE * pFile;
    pFile = fopen("log.log", "a");

    if(pFile == NULL)
    {
        perror("Fail to fopen\n");
        return;
    }

    va_list args;
    va_start(args, fmt);
    
    char szBuf[1024] = { 0 };

    vsnprintf(szBuf, 1024, fmt, args);

    fprintf(pFile, "%s", szBuf);

    fclose(pFile);

    va_end(args);

}

int main(void)
{
    //Test_fwrite();

    //Test_fread();

    //TestBuf();

    //TestPrint();

    //printf("Sum1: %d\n", sum(2, 2, 3));
    //printf("Sum2: %d\n", sum(4, 2, 3, 4, 5));
    //printf("Sum5: %d\n", sum(3, 2, 3, 4));

    writeLog("This is log %d\n", 1);
    writeLog("Hello!! %s, %d\n", "YOU", 2);

    return 0;
}