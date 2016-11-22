#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void TestTime();
void TestMicroTime();
void TestSignal();
void TestSignalIgnore();
void TestSigaction();
void TestSigaction_Fork();

int main(void)
{
	TestSigaction();

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



// 通过忽略信号或自己处理来防止僵尸进程
void waitFun(int sig)
{
	printf("Receivce signal %d\n", sig);
	wait(NULL);
}

void TestSignalIgnore()
{
	printf("Pid %d\n", getpid());

	pid_t pid = fork();
	if(pid == 0)
	{
		printf("Child pid: %d\n", getpid());

		_exit(0);

	}
	else
	{
		// 忽略子进程传给父进程的信号
		// 实质上这会由init --user来释放资源
		signal(SIGCHLD, SIG_IGN);

		// 捕获信号，自己处理
		//signal(SIGCHLD, waitFun);

		while(1);
	}
	
}



// 使用sigaction()函数处理更多的信息
void sigactionWaitFun(int sig)
{
	printf("Receive signal %d\n", sig);
	wait(NULL);
}

void complexSigactionFun(int sig, siginfo_t * pInfo, void * ex)
{
	printf("Receive signal %d\n", sig);
	printf("pid: %d\tuid: %d\n", pInfo->si_pid, pInfo->si_uid);
	printf("Extra data: %d\n", pInfo->si_value.sival_int);

	sleep(10);

}

void TestSigaction()
{
	printf("pid: %d\n", getpid());

	struct sigaction act = { 0 };
	struct sigaction old = { 0 };
	
	//act.sa_handler = sigactionWaitFun;
	//sigaction(SIGUSR1, &act, &old);

	// sigaction()函数使用第二类函数指针，需要置位SA_SIGINFO
	act.sa_flags |= SA_SIGINFO;
	// 捕获信号后函数处理期间，如果捕获到多个同样的信号，将它们同时处理
	act.sa_flags |= SA_NODEFER;
	// 捕获信号并处理完成之后，恢复缺省处理
	act.sa_flags |= SA_RESETHAND;

	// 设置处理函数
	act.sa_sigaction = complexSigactionFun;

	// 设置在函数处理期间，暂时屏蔽SIGINT信号(Ctrl+C)
	// 函数处理结束之后再处理SIGINT信号
	sigaddset(&act.sa_mask, SIGINT);

	sigaction(SIGUSR2, &act, &old);


	// 发送信号给进程，同时附加上信息
	union sigval sigv;
	sigv.sival_int = 123456;
	// 自己给自己发送信号
	sigqueue(getpid(), SIGUSR2, sigv);


	while(1);

}



// fork，获取子进程信息
void TestSigaction_Fork()
{
	printf("Parent Pid: %d\n", getpid());

	struct sigaction act = { 0 };
	struct sigaction old = { 0 };

	// 接收到SIGCHLD时做处理
	act.sa_handler = sigactionWaitFun;
	
	// 设置子进程暂停时，不发送SIGCHLD信号通知父进程
	act.sa_flags |= SA_NOCLDSTOP;

	// 设置子进程退出时，不会变成僵尸进程
	act.sa_flags |= SA_NOCLDWAIT;

	sigaction(SIGCHLD, &act, &old);

	pid_t pid = fork();
	if(pid == 0)
	{
		printf("Child %d [And Pause]\n", getpid());

		// 在没有设置SA_NOCLDSTOP时
		// 子进程暂停时，也会发送SIGCHLD给父进程
		// 父进程不清楚子进程是否已经退出，在处理函数中一直等待wait函数返回而被阻塞
		//kill(getpid(), SIGSTOP);

		printf("Continue...\n");
		_exit(0);
	}

	while(1)
	{
		printf(".");
	}

}
