#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

void TestAlarm();
void TestAlarmSleep();
void TestMicroTimer();


int main(void)
{
	TestAlarmSleep();

	return 0;
}


// 定时器，按秒定时
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


// sleep对定时器的影响
void PrintDot()
{
	fprintf(stderr, ".");
	alarm(1);
}

void TestAlarmSleep()
{
	alarm(1);

	signal(SIGALRM, PrintDot);

	while(1)
	{
		// 任何信号都会打断sleep，不会睡眠指定的秒数。
		// 所以fprintf会按alarm(1)每秒输出，不会睡眠100秒再输出。
		sleep(100);
		fprintf(stderr, "@");
	}

}

// 定时器，按微秒定时
void MicroTimerFun(int sig)
{
	switch(sig)
	{
	case SIGALRM:
		printf("ITIMER_REAL...\n");
		break;
	
	case SIGVTALRM:
		printf("ITIMER_VIRTUAL...\n");
		break;

	case SIGPROF:
		printf("ITIMER_PROF...\n");
		break;
	
	default:
		printf("Unknown...\n");
		break;
	}

}

void TestMicroTimer()
{
	struct itimerval itv;
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 500 * 1000;	// 0.5s

	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 500 * 1000;

	setitimer(ITIMER_REAL, &itv, NULL);
	setitimer(ITIMER_VIRTUAL, &itv, NULL);
	setitimer(ITIMER_PROF, &itv, NULL);

	signal(SIGALRM, MicroTimerFun);
	signal(SIGVTALRM, MicroTimerFun);
	signal(SIGPROF, MicroTimerFun);

	while(1)
	{
		usleep(100 * 1000);	// 100ms
		printf(".");
	}

}



