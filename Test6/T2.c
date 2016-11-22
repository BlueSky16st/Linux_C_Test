#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void TestAlarm();


int main(void)
{
	TestAlarm();

	return 0;
}


// 定时器
long long x = 0;
void AlarmFun(int sig)
{
	printf("timer hit: x = %lld\n", x);

	// 设置多个定时器
	static n = 0;
	n++;
	if(n % 2 == 0)		// 2秒
	{

	}
	if(n % 5 == 0)		// 5秒
	{

	}

	alarm(1);
}

void TestAlarm()
{

	// 参数1指定秒数，返回上一个定时时间
	// 到时后会发送SIGALRM信号，定时器是一次性的
	alarm(1);
	// 设置SIGALRM信号处理函数
	signal(SIGALRM, AlarmFun);

	while(1)
	{
		x++;
	}

}

