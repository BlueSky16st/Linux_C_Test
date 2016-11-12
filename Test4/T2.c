#include <stdio.h>
#include <unistd.h>
#include <termio.h>
#include <string.h>

void TestIOFlag();

int main(void)
{
	TestIOFlag();
	
	return 0;
}

void TestIOFlag()
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