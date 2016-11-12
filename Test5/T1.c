#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void TestPID();
void TestFork();

int main(void)
{
    TestFork();

    return 0;
}

// 获取本程序的PID、UID、GID
void TestPID()
{
    printf("My PID: %d\n", getpid());
    printf("Parent PID: %d\n", getppid());
    printf("Group PID: %d\n", getpgrp());

    printf("\n");

    printf("UID: %d\n", getuid());
    printf("EUID: %d\n", geteuid());
    printf("GID: %d\n", getgid());
    printf("EGID: %d\n", getegid());
}

// 
void TestFork()
{
    pid_t pid = getpid();
    printf("My PID:%d\n", pid);

    // 复制出一个新的进程，此时共有2个程序同时执行接下来的代码
    pid = fork();
    if(pid < 0)
    {
        perror("Fail to fork()");
        return;
    }
    
    if(pid == 0)    // 如果pid为0，则是子进程。
    {
        // 执行这句的进程是子进程
        printf("Child PID:%d\n", getpid());
    }
    else            // pid为父进程
    {
        // 执行这句的进程是父进程
        printf("Parent PID:%d\n", getpid());
    }

}