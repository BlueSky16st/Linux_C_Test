#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

void TestPipe();
void TestDblPipe();
void TestDupPipe();
void Testpopen();
void Testpopen2();
void TestFIFO();

int main(void)
{
	TestFIFO();

	return 0;
}


// 管道通信，实现父子进程间通信
void TestPipe()
{
	printf("PID:%d\n", getpid());

	int fd[2];
	if(pipe(fd) != 0)
	{
		perror("Error fd\n");
		return;
	}

	printf("fd[0]=%d\tfd[1]=%d\n", fd[0], fd[1]);

	pid_t pid = fork();
	char szBuf[100];
	if(pid == 0)
	{
		printf("Child pid:%d\n", getpid());

		// 子进程关闭写文件描述符
		close(fd[1]);
		
		while(1)
		{
			memset(szBuf, 0, 100);
			read(fd[0], szBuf, 100);	// 从读文件描述符中读取字符
			printf("Child reveive:%s\n", szBuf);
		}
		close(fd[0]);

	}
	else
	{
		// 父进程关闭读文件描述符
		close(fd[0]);
		
		while(1)
		{
			usleep(100);
			fprintf(stderr, "Send:");
			//scanf("%s", szBuf);

			memset(szBuf, 0, 100);
			read(0, szBuf, 100);
			write(fd[1], szBuf, strlen(szBuf));	// 从写文件描述符中写入字符
		}
		close(fd[1]);
	}

}


// 测试双向管道通信
void TestDblPipe()
{
	printf("PID:%d\n", getpid());

	int fda[2];
	int fdb[2];

	if(pipe(fda))
	{
		perror("Error fda");
		return;
	}
	if(pipe(fdb))
	{
		perror("Error fdb");
		return;
	}

	pid_t pid = fork();
	char szBuf[100];
	if(pid == 0)
	{
		printf("Child pid:%d\n", getpid());

		close(fda[1]);
		close(fdb[0]);

		while(1)
		{
			memset(szBuf, 0, 100);
			read(fda[0], szBuf, 100);

			int i;
			for(int i = 0; i < strlen(szBuf); i++)
				szBuf[i] = toupper(szBuf[i]);

			write(fdb[1], szBuf, strlen(szBuf));
		}

		close(fda[0]);
		close(fdb[1]);

	}
	else
	{
		close(fda[0]);
		close(fdb[1]);

		while(1)
		{
			usleep(100);
			fprintf(stderr, "Send:");

			memset(szBuf, 0, 100);
			read(0, szBuf, 100);

			write(fda[1], szBuf, strlen(szBuf));
			read(fdb[0], szBuf, 100);

			printf("Upper: %s", szBuf);

		}
		close(fda[1]);
		close(fdb[0]);
	}

}


// 重定向管道通信
void TestDupPipe()
{
	int fd[2];

	if(pipe(fd))
	{
		perror("Error fd\n");
		return;
	}

	pid_t pid = fork();
	if(pid == 0)
	{
		close(fd[1]);

		dup2(fd[0], 0);	// 将stdin重定向到fd[0]

		// 管道有数据时,将读取到的数据当作grep的输入数据
		execlp("grep", "grep", "pts", NULL);
	}
	else
	{
		close(fd[0]);

		int f = open("a.txt", O_RDONLY);
		if(f < 0)
		{
			printf("File:%s, Line:%d\n", __FILE__, __LINE__);
			perror("Error open");
			return;
		}
		
		char szLine[100];

		while(1)
		{
			int res = read(f, szLine, 100);
			if(res < 0)
			{
				perror("Error read");
			}
			else if(res == 0)
			{
				break;
			}
			else
			{
				// 将a.txt文件中读取到的数据写入到管道中
				write(fd[1], szLine, strlen(szLine));
			}
		}

		close(fd[1]);
	}

}


// popen()
void Testpopen()
{
	int arr[] = { 1, 6, 7, 5, 4, 9, 2 };

	// "w"由父进程写入sort进程中
	// "r"由sort进程写入到父进程中
	FILE * pFile = popen("sort -n", "w");
	if(!pFile)
	{
		perror("Error popen");
		return;
	}

	for(int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		fprintf(pFile, "%d\n", arr[i]);
	}
	
	pclose(pFile);

}

// 通过popen,实现ps -ef | grep pts
void Testpopen2()
{
	FILE * pPs = popen("ps -ef", "r");
	FILE * pGrep = popen("grep pts", "w");

	if(!pPs)
	{
		perror("Error pPs");
		return;
	}
	if(!pGrep)
	{
		perror("Error pGrep");
		pclose(pPa);
		return;
	}

	char szBuf[256];
	while(fgets(szBuf, 256, pPs))
	{
		fprintf(pGrep, "%s\n", szBuf);
	}

	pclose(pPs);
	pclose(pGrep);

}

// 创建管道文件
void TestFIFO()
{
	char mode = 'r';	// 选择读或写模式，"w"或"r"
	mode = getchar();

	char szFile[] = "pnod1";	// 管道文件
	char szBuf[1024] = { 0 };
	int fd, len;
	struct stat info;

	if(stat(szFile, &info) != 0)
	{
		if(mkfifo(szFile, 0664) != 0)
		{
			perror("Error");
			return;
		}
	}
	else if(!S_ISFIFO(info.st_mode))
	{
		printf("Cannot create file\n");
		return;
	}

	if(mode == 'r')
	{
		// 读
		fd = open(szFile, O_RDONLY);
		if(fd < 0)
		{
			perror("Error");
			return;
		}

		while(1)
		{
			memset(szBuf, 0, 1024);
			len = read(fd, szBuf, 1024);
			if(len < 0)
			{
				perror("Error");
				break;
			}
			else if(len == 0)
			{
				perror("Error");
				break;
			}

			printf("Receive: %s", szBuf);
		}

		close(fd);

	}
	else if(mode == 'w')
	{
		// 写
		fd = open(szFile, O_WRONLY);
		if(fd < 0)
		{
			perror("Error");
			return;
		}

		while(1)
		{
			fprintf(stderr, "Send: ");

			memset(szBuf, 0, 1024);
			read(0, szBuf, 1024);

			write(fd, szBuf, strlen(szBuf));
		}

		close(fd);

	}

}
