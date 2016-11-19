#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

void TestEnv();
void TestWait();
void TestZomb();
void TestKill();
void TestTrusteeship();
void TestDaemon();

int main(int argc, char * argv[], char * envp[])
{
    TestDaemon();

    return 0;
}

// 打印环境变量
// environ是UNIX的全局变量，保存环境指针数组
extern char ** environ;
void TestEnv()
{
    // 输出所有环境变量
	char ** p = environ;
	while(*p)
	{
		printf("%s\n", *p);
		p++;
	}
	printf("\n\n");
	

	// 输出指定环境变量
	printf("PATH:%s\n", getenv("PATH"));
	
	
	// 设置环境变量
	putenv("AAA=Hello world!");
	// 创建一个子进程，环境变量继承父进程的环境变量
    // 如果使用exec时指定了环境变量，则子进程的环境变量不会继承父进程的环境变量
	pid_t pid = fork();
	if(pid == 0)
	{
		printf("Child AAA = %s\n", getenv("AAA"));
	}
	else
	{
		printf("Parent AAA = %s\n", getenv("AAA"));
	
	    // AAA后不加任何字符，删除环境变量
	    putenv("AAA");

        usleep(100000);
	}
	
	printf("now AAA = %s\n", getenv("AAA"));
	
}

// 测试wait函数，父进程等待子进程结束后继续执行
// 多个子进程需要多个wait函数调用
// 打印信号与退出码
// 如果没有子进程，wait()返回0
void TestWait()
{
	pid_t pid1, pid2;
	
	printf("Parent: Start [%d]\n", getpid());
	
	pid1 = fork();
	if(pid1 == 0)
	{
		printf("child 1 [%d]\n", getpid());
		//exit(1);  // 注释掉，子进程会继续执行接下来的代码，在pid2创建孙进程。
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
		// iState & 0xFF：接收到的信号值
		// (iState >> 8) & 0xFF：退出码
		printf("pid=[%d] SIG=%d exitCode=%d\n", getpid(), iState & 0xFF, (iState >> 8) & 0xFF);
	}
	
    // 不接收信号和退出码
	wait(NULL);
	
	printf("Parent: End [%d]\n", getpid());
	
}

// 僵尸进程
// 如果子进程已经结束，父进程在处理自己的代码。
// 则子进程会变成僵尸进程(defunct)
// 因为子进程的进程资源没有被父进程释放
void TestZomb()
{
	pid_t pid = fork();
	
	if(pid == 0)
	{
		printf("Child\n");
        exit(0);
	}
    
    printf("Parent\n");

    // 使用wait，防止僵尸进程产生，但阻塞父进程
    //wait(NULL);

    while(1);
	
}

// 使用kill()关闭进程
void TestKill()
{
	pid_t pid1, pid2;
    printf("Parent pid: %d\n", getpid());
    
    pid1 = fork();
    if(pid1 == 0)
    {
        printf("Child pid: %d\n", getpid());
        pid2 = fork();
        if(pid2 == 0)
        {
            printf("Sub Child pid: %d\n", getpid());
            printf("[%d] Kill parent %d\n", getpid(), getppid());
            
            // 杀死父进程，pid2的父进程id变为[init --user]进程的id，由[init --user]释放子进程资源
            kill(getppid(), 9);

            usleep(100);
            printf("Now parent: %d\n", getppid());
            exit(0);
        }
        else
        {
            sleep(1);
        }
        printf("End\n");
        exit(1);
    }
    
    int iState;
    pid_t pid = wait(&iState);
    if(pid > 0)
    {
		printf("pid=[%d] SIG=%d exitCode=%d\n", getpid(), iState & 0xFF, (iState >> 8) & 0xFF);
    }
	
}

// 使用托管
// 将子进程托管给[init --user]进程，由该进程负责释放资源
// 创建子进程，再由子进程创建孙进程，子进程退出。则此时孙进程的父进程变为[init --user]进程。
void TestTrusteeship()
{
	pid_t pid = fork();
	
	if(pid == 0)
	{
        printf("Child\n");
        pid_t pid1 = fork();
        if(pid1 == 0)
        {
		    printf("Sub Child\n");

            // exec(...);
            while(1);

            exit(0);
        }
        else
        {
            // fork()后马上退出。
            exit(0);
        }
	}
    
    wait(NULL);

    while(1);
	
}

// 守护进程（精灵进程）
// 运行在后台的特殊进程，不存在控制终端，守护系统正常运行
// 
// 步骤：
// 1、后台运行
//      使用托管
// 2、脱离控制终端（伪终端）
//      pid = setsid()
// 3、把当前工作目录更改为"/"，但不是必须的
//      chdir()，防止对当前目录的某些操作不能执行
// 4、关闭文件描述符
//      重定向或close()：stdin、stdout、stderr
// 5、设置/清除文件创建掩码
//      umask(0)：清除从父进程继承的文件创建掩码

// 创建守护进程，保持[gedit]程序的打开。
void TestDaemon()
{
    pid_t pid = fork();
    if(pid > 0)
    {
        // 直接退出父进程
        exit(0);
    }

    // 接下来的都由子进程执行
    // 此时子进程的父进程为[init --user]
    printf("parent: %d\n", getppid());

    // 脱离控制终端，设置为"?"
    pid = setsid();
    if(pid < 0)
    {
        // 设置失败
        perror("Fail\n");
        return;
    }

    // 把当前工作目录更改为"/"，但不是必须的。
    chdir("~/home/home");

    // 关闭文件描述符
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // 打开并保持[gedit]
    do
    {
        pid = fork();
        if(pid == 0)
        {
           execlp("gedit", "gedit", "demo.txt", NULL);
        }

        // 一旦wait函数返回，则再次运行[gedit]程序
        wait(NULL);
    } while(1);

}