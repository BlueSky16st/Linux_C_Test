#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void TestTime();
void TestMicroTime();
void TestSignal();

int main(void)
{
    TestMicroTime();

    return 0;
}

// 获取时间
void TestTime()
{
    time_t iTime;
    
    // 2种方法，得到的是1900/1/1至今的秒数
    iTime = time(NULL);
    time(&iTime);

    printf("iTime: %d\n", (int)iTime);

    // 将秒数转为时间结构体
    // 年需要加1900,月需要加1。
    struct tm * tTime;
    tTime = localtime(&iTime);
    printf("%04d/%02d/%02d %02d:%02d:%02d week %d\n", 
        tTime->tm_year + 1900, tTime->tm_mon + 1, tTime->tm_mday, 
        tTime->tm_hour, tTime->tm_min, tTime->tm_sec,
        tTime->tm_wday);

    // 自定义格式时间
    char szTime[40];
    strftime(szTime, 40, "Time1: %Y/%m/%d %H:%M:%S", tTime);
    printf("%s\n", szTime);
    strftime(szTime, 40, "Time2: %x %X week:%w", tTime);
    printf("%s\n", szTime);

}

// 获取微秒级时间
void TestMicroTime()
{
    struct timeval tv;
    struct timezone tz;

    // 获取微秒级时间和时区
    gettimeofday(&tv, &tz);

    struct tm * tTime = localtime(&(tv.tv_sec));

    printf("%04d/%02d/%02d %02d:%02d:%02d.%d\n", 
        tTime->tm_year + 1900, tTime->tm_mon + 1, tTime->tm_mday, 
        tTime->tm_hour, tTime->tm_min, tTime->tm_sec, (int)tv.tv_usec);

    printf("Zone: minute=%d dst=%d\n", tz.tz_minuteswest / 60, tz.tz_dsttime);


    // 计算微秒级的时间间隔
    struct timeval tv1, tv2;

    gettimeofday(&tv1, NULL);
    int i = 0, sum = 0;
    while(i++ < 0xffffff)
    {
        sum += i;
    }
    gettimeofday(&tv2, NULL);

    int usec = tv2.tv_usec - tv1.tv_usec;
    if(usec < 0)
        usec += 1000000;
    printf("time(us): %d\n", usec);

}



// 信号处理
// typedef void (*pFun)(int);
// pFun signal(int sig, pFun fun);
// signal()返回旧的处理sig信号的函数

// 捕获信号后调用的函数
void fun(int sig)
{
    // 输出信号值
    printf("Recive: %d [%s]\n", sig, strsignal(sig));

    // 还原默认处理
    signal(SIGINT, SIG_DFL);

}

void TestSignal()
{
    printf("pid:[%d]\n", getpid());

    // 捕获SIGINT信号，并调用函数fun
    signal(SIGINT, fun);

    // 捕获SIGTERM信号，并调用函数fun
    signal(SIGTERM, fun);
    // 忽略SIGTERM信号，不做任何响应
    signal(SIGTERM, SIG_IGN);

    // SIGKILL信号无法捕获。
    signal(SIGKILL, fun);

    while(1);

}