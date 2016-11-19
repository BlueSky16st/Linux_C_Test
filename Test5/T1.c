#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

void TestPID();
void TestFork();
void TestVFork();
void TestExec();
void TestEnv();
void TestWait();
void TestZomb();

int main(int argc, char * argv[], char * envp[])
{
    TestExec();

    return 0;
}

// 获取本程序的PID、UID、GID
void TestPID()
{
    printf("My PID: %d\n", getpid());
    printf("Parent PID: %d\n", getppid());
    printf("Group PID: %d\n", getpgrp());

    printf("\n");

    printf("UID: %d\n", getuid());		// 进程的用户ID
    printf("EUID: %d\n", geteuid());	// 进程的有效用户ID
    printf("GID: %d\n", getgid());		// 进程的组ID
    printf("EGID: %d\n", getegid());	// 进程的有效组ID


	// 使用root权限创建a.txt和T1程序。
	// 如果程序使用普通用户运行，则会出现没有权限。
	// 使用root才可以访问该文件。
	// root通过给程序加s权限，使得程序使用普通用户运行时暂时获得root权限，可以访问a.txt
	// chmod u+s T1
	// 此时euid为root的id
	int fd = open("a.txt", O_RDONLY);
	if(fd < 0)
	{
		perror("Faile to open");
		return;
	} 
	char szBuf[100] = { 0 };
	read(fd, szBuf, 100);

	printf("szBuf: %s\n", szBuf);

	close(fd);

}

// fork()复制新的进程
// 使用写时拷贝技术。
// 复制了父线程的所有资源。
void TestFork()
{
    pid_t pid = getpid();
	int x = 10;
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
		printf("x = %d\n", x);
		
		// 将新的程序作为父进程的子进程
		// 所有父进程的资源被替换成新的程序的资源
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

// 使用vfork()，共享父子进程的资源，没有复制资源
// 父进程会等待子进程调用exec函数簇或函数返回时继续执行。
// 如果子进程使用return或exit()返回，父进程可能会崩溃。
// 子进程需要使用_exit()函数进行返回。
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
		usleep(1000);
		x = 20;	// 修改了父进程的资源

        printf("Child PID:%d\n", getpid());
		printf("x = %d\n", x);
		
		// 将新的程序作为父进程的子进程
		//execl("./T2", "./T2", NULL);
		
		// 使用exec之后，之后的语句不会再执行
		// 以下这个不会输出
		printf("Child\n");
		
    }
    else
    {
        printf("Parent PID:%d\n", getpid());
		printf("x = %d\n", x);
    }

	//while(1);
	//_exit(0);

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
		execl("/bin/ls", "ls", "-l", NULL);
		
		// 使用execv()，第二个参数是参数数组
		//char * argv[10] = { "ls", "-l" };
		//execv("/bin/ls", argv);

		// 在环境变量PATH中的所有目录查找该程序名，执行第一个该程序名的程序
		// 如果程序名不在环境变量PATH中的目录下时，执行失败
		//execlp("ls", "ls", "-l", NULL);
		
		// 自定义环境变量，并在环境变量的目录中查找该程序名
		//char * env[128] = { "/mnt/c/Software/Programming/ProgramProject/Git/Linux_C_Test/Test5" };
		//execle("T2", "T2", NULL, env);
		
		// 不会执行这条语句
		printf("End\n");
	}
	else
	{
		printf("Parent: %d\n", getpid());
		
		usleep(100000);
	}
	
}