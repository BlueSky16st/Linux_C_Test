#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// 使用open()
void Test_open()
{
	int fd;	// 文件描述符
	
	// 创建并打开文件，权限：0660（rw-rw----）
	// O_RDONLY：只读
	// O_CREAT：文件不存在时创建
	// O_EXCL：限定必须创建文件，否则如果已存在文件，则打开失败
	fd = open("1.txt", O_RDONLY | O_CREAT | O_EXCL, 0660);
	if(fd < 0)
	{
		perror("Fail to open");
		return;
	}
	char szBuf[100] = { 0 };
	read(fd, szBuf, sizeof(szBuf));
	close(fd);
	
	printf("szBuf: %s\n", szBuf);
	
}

// read(int fd, void * buf, unsigned int byte)
// write(int fd, )
void Test_read_write()
{
	char szBuf[100] = { 0 };
	
	// 相当于从stdin流中赋值给szBuf，参数1的STDIN_FILENO相当于0
	read(STDIN_FILENO, szBuf, sizeof(szBuf));
	
	//printf("input: %s\n", szBuf);
	// 相当于szBuf输出到stdout流中，参数1的STDOUT_FILENO相当于1
	write(STDOUT_FILENO, szBuf, strlen(szBuf));
	
	memset(szBuf, 0, 100);
	memcpy(szBuf, "Hello\n\0world!\n\0", sizeof("Hello\n\0world!\n\0"));
	// STDERR_FILENO相当于2
	// write()函数相当于直接将内存中的值打印出来，不会因为有'\0'而结束打印。
	write(STDERR_FILENO, szBuf, sizeof(szBuf));
	
}

// 打开字符设备
void Test_tty()
{
	// 打开"tty2"设备
	int fd = open("/dev/tty2", O_WRONLY);
	if(fd < 0)
	{
		perror("Fail to open");
		return;
	}
	
	char szBuf[100];
	int i = 0;
	while(i++ < 10)
	{
		sprintf(szBuf, "This is line %d\n", i);
		write(fd, szBuf, strlen(szBuf));
	}
	close(fd);
	
}

// 使用dup(int fd)重定向
void Test_dup()
{
	// fd相当于stdout
	int fd = dup(STDOUT_FILENO);
	char szBuf[100] = "Hello world\n";
	
	// 向fd中输入数据相当于向stdout输入数据
	write(fd, szBuf, strlen(szBuf));
	
	close(fd);
	
}

// dup2(int fd1, int fd2)
void Test_dup2()
{
	int fd = open("1.txt", O_WRONLY);
	if(fd < 0)
	{
		perror("Fail to open");
		return;
	}
	
	// 备份STDOUT_FILENO
	int tempfd = dup(STDOUT_FILENO);
	
	// 将STDOUT_FILENO重定向到fd
	dup2(fd, STDOUT_FILENO);
	
	printf("Hello world\n");
	char szBuf[] = "Hello C/C++\n";
	write(STDOUT_FILENO, szBuf, sizeof(szBuf));
	
	write(STDOUT_FILENO, "Hello Linux\n", strlen("Hello Linux\n"));
	
	// 还原STDOUT_FILENO
	dup2(tempfd, STDOUT_FILENO);
	printf("Hello!!\n");
	
	close(fd);
	
}

// getcwd(char * path, size_t size)：获取当前路径
// chdir(char * path)：设置当前工作目录，相当于cd命令，可以使用相对路径或绝对路径
// fchdir(int fd)：设置当前工作目录到fd的当前目录
void Test_Dir()
{
	char szBuf[100];
	// 返回当前路径，同时赋值给szBuf
	// 获取失败返回NULL
	if(NULL == getcwd(szBuf, 100))
	{
		perror("Fail to getcwd");
		return;
	}
	printf("szBuf: %s\n", szBuf);
	
	// 进入上一级目录
	chdir("../");
	getcwd(szBuf, 100);
	printf("szBuf: %s\n", szBuf);
	
	// 设置当前工作目录
	int fd = open("/mnt/c/Software/Programming/ProgramProject/Git/Linux_C_Test/Test2/", O_RDONLY);
	if(fchdir(fd) == -1)
		perror("Fail to fchdir");
	getcwd(szBuf, 100);
	printf("szBuf: %s\n", szBuf);
	
}

int main(void)
{
	//Test_open();
	
	//Test_read_write();
	
	//Test_tty();
	
	//Test_dup();
	
	//Test_dup2();
	
	Test_Dir();
	
	return 0;
}