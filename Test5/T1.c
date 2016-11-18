#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


void TestPID();
void TestFork();
void TestVFork();
void TestExec();
void TestEnv();
void TestWait();
void TestZomb();

int main(int argc, char * argv[], char * envp[])
{
    TestWait();

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

// fork()复制新的进程
// 使用了写时拷贝技术
void TestFork()
{
    pid_t pid = getpid();
	int x = 10;
    printf("My PID:%d\n", pid);

    // 复制出一个新的进程，此时共有2个程序同时执行接下来的代码
	// 复制了父线程的所有资源。
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
		printf("x = %d\n", x);
		
		// 在子进程中运行T2程序
		execl("./T2", "./T2", NULL);
		
    }
    else            // pid为父进程
    {
		x = 20;
        // 执行这句的进程是父进程
        printf("Parent PID:%d\n", getpid());
		printf("x = %d\n", x);
    }
	
	while(1);

}

// 使用vfork
// 父进程会等待子进程调用_exit()或exec函数簇时继续执行。
// 如果子进程使用return或exit()返回，父进程会崩溃。
void TestVFork()
{
    pid_t pid = getpid();
	int x = 10;
    printf("My PID:%d\n", pid);

    pid = vfork();
    if(pid < 0)
    {
        perror("Fail to vfork()");
        return;
    }
    
    if(pid == 0)
    {
        // 执行这句的进程是子进程
        printf("Child PID:%d\n", getpid());
		printf("x = %d\n", x);
		
		// 将新的程序作为父进程的子进程
		// 所有资源被替换成新的程序的资源
		//execl("./T2", "./T2", NULL);
		
		// 使用exec之后，之后的语句不会再执行
		// 以下这个不会输出
		printf("Child\n");
		
    }
    else
    {
		x = 20;
        // 执行这句的进程是父进程
        printf("Parent PID:%d\n", getpid());
		printf("x = %d\n", x);
    }

}

// exec()函数簇
void TestExec()
{
	pid_t pid = fork();
	if(pid == 0)
	{
		printf("Child: %d\n", getpid());
		
		// 执行ls命令
		
		// 使用execl()，第一个参数输入路径
		// 第二个参数开始传入参数，直到最后，再以NULL结尾
		//execl("/bin/ls", "ls", "-l", NULL);
		
		// 在环境变量PATH中的所有目录查找该程序名，执行第一个找到该程序名的程序
		// 如果程序名不在环境变量PATH中的目录下时，执行失败
		//execlp("ls", "ls", "-l", NULL);
		
		// 自定义环境变量，并在环境变量的目录中查找该程序名
		char * env[128] = { "/mnt/c/Software/Programming/ProgramProject/Git/Linux_C_Test/Test5" };
		execle("T2", "T2", NULL, env);
		
		// 使用execv()，第二个参数是参数数组
		//char * argv[10] = { "ls", "-l" };
		//execv("/bin/ls", argv);
		
		// 不会执行这条语句
		printf("End\n");
	}
	else
	{
		printf("Parent: %d\n", getpid());
		
		usleep(100000);
	}
	
}

// 打印环境变量
extern char ** environ;
void TestEnv()
{
	// environ是全局变量，保存所有环境变量
	char ** p = environ;
	while(*p)
	{
		printf("%s\n", *p);
		p++;
	}
	printf("\n\n");
	
	// 读取指定环境变量
	printf("PATH:%s\n", getenv("PATH"));
	
	
	
	// 设置环境变量
	putenv("AAA=Hello world!");
	// 创建一个子进程
	pid_t pid = fork();
	if(pid == 0)
	{
		printf("AAA = %s\n", getenv("AAA"));
	}
	else
	{
		printf("Parent1 AAA = %s\n", getenv("AAA"));
	}
	
	// AAA后不加任何字符，删除环境变量
	putenv("AAA");
	
	printf("now AAA = %s\n", getenv("AAA"));
	
}

// 测试wait函数
// 打印信号与退出码
void TestWait()
{
	pid_t pid1, pid2;
	
	printf("Start [%d]\n", getpid());
	
	pid1 = fork();
	if(pid1 == 0)
	{
		printf("child 1 [%d]\n", getpid());
		exit(1);
	}
	
	pid2 = fork();
	if(pid2 == 0)
	{
		printf("child 2 [%d]\n", getpid());
		exit(2);
	}
	
	int iState;
	pid_t pid = wait(&iState);
	if(pid > 0)
	{
		// iState & 0xFF：信号码
		// (iState >> 8) & 0xFF：退出码
		printf("[%d] SIG=%d exitCode=%d\n", getpid(), iState & 0xFF, (iState >> 8) & 0xFF);
	}
	
	wait(NULL);
	
	printf("End [%d]\n", getpid());
	
}


void TestZomb()
{
	pid_t pid = fork();
	
	if(pid == 0)
	{
		
	}
	
}