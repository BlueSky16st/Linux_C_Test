#include <stdio.h>
#include <unistd.h>
#include <termio.h>
#include <string.h>

void TestIOFlag1();
void TestIOFlag2();
void TestIOFlag3();
void TestIOFlag4();
void TestIOFlag5();

int main(void)
{
	TestIOFlag5();
	
	return 0;
}

// 在终端输入时大写字母自动转小写
void TestIOFlag1()
{
	struct termios old, new;
	char szBuf[100];
	
	// 备份终端属性
	ioctl(STDIN_FILENO, TCGETA, &old);
	new = old;
	
	// 置位标识位：大写自动转小写
	new.c_iflag |= IUCLC;
	
	// 设置新的属性
	ioctl(STDIN_FILENO, TCSETA, &new);
	
	scanf("%s", szBuf);
	printf("szBuf: %s\n", szBuf);
	
	// 还原终端属性
	ioctl(STDIN_FILENO, TCSETA, &old);
	
	scanf("%s", szBuf);
	printf("szBuf: %s\n", szBuf);
	
}

// 在终端输出时小写字母自动转大写
void TestIOFlag2()
{
	struct termio old, new;

	ioctl(STDOUT_FILENO, TCGETA, &old);
	new = old;

	// 置位标识位，小写转大写
	new.c_oflag |= OLCUC;

	ioctl(STDOUT_FILENO, TCSETA, &new);

	printf("Hello, Linux 1!\n");

	ioctl(STDOUT_FILENO, TCSETA, &old);

	printf("Hello, Linux 2!\n");

}

// 输入字符，输出相应的ASCII码
void TestIOFlag3()
{
	struct termio old, new;

	ioctl(STDIN_FILENO, TCGETA, &old);
	new = old;
	// 删除c_lflag变量中的ICANON，输入无缓冲
	new.c_lflag &= ~ICANON;
	ioctl(STDIN_FILENO, TCSETA, &new);

	char c = 0;
	while(c != '\n')
	{
		c = getchar();
		printf("-->%d\n", c);
	}
	
	ioctl(STDIN_FILENO, TCSETA, &old);

}

// 不回显字符
void TestIOFlag4()
{
	struct termio old, new;
	
	ioctl(STDIN_FILENO, TCGETA, &old);
	new = old;
	// 删除c_lflag变量中的ECHO，字符不回显
	new.c_lflag &= ~ECHO;
	// 加上属性ECHONL，ECHO关闭时，回车会正常换行
	new.c_lflag |= ECHONL;

	ioctl(STDIN_FILENO, TCSETA, &new);

	char szPass[100] = { 0 };
	fprintf(stderr, "Password:");

	scanf("%s", szPass);

	printf("Your password is %s\n", szPass);

	ioctl(STDIN_FILENO, TCSETA, &old);

}

// 设置输入字符限制和超时时间，因为输入有缓冲，所以需要设置输入无缓冲
void TestIOFlag5()
{
	struct termio old, new;
	char szBuf[100] = { 0 };

	ioctl(STDIN_FILENO, TCGETA, &old);
	new = old;
	new.c_lflag &= ~ICANON;	// 设置无缓冲

	//new.c_cc[VMIN] = 10;	// 最多输入10个字符
	new.c_cc[VMIN] = 0;		// 不限定

	new.c_cc[VTIME] = 50;	// 5秒超时时间，如果c_cc[VMIN]等于0，read()只读取1个字符，scanf()不限制字符，直到回车或超时
	//new.c_cc[VTIME] = 0;	// 不超时，如果c_cc[VMIN]等于0,read()直接返回，不读取任何字符，scanf()不限制字符和时间，直到回车

	ioctl(STDIN_FILENO, TCSETA, &new);

	fprintf(stderr, "input:");

	read(STDIN_FILENO, szBuf, 100);
	//scanf("%s", szBuf);

	printf("%s\n", szBuf);

	ioctl(STDIN_FILENO, TCSETA, &old);

}