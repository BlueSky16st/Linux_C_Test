#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void TestFtok();
void TestMsgget();

int main(void)
{
	TestMsgget();


	return 0;
}


void TestFtok()
{
	key_t key;
	key = ftok("a.txt", 1);
	if(key == -1)
	{
		perror("Error");
		return;
	}
	printf("[1] key: %d\n", key);

	key = ftok("a.txt", 2);
	if(key == -1)
	{
		perror("Error");
		return;
	}
	printf("[2] key: %d\n", key);

	key = ftok("a.txt", 1);
	if(key == -1)
	{
		perror("Error");
		return;
	}
	printf("[3] key: %d\n", key);

}

// 使用msget，创建一个消息队列
void TestMsgget()
{
	key_t key = ftok("a.txt", 1);
	if(key == -1)
	{
		perror("Error");
		return;
	}
	printf("Key: %d\n", key);

	// IPC_PRIVATE：创建key = 0的消息队列，可以同时存在多个，但不能与其它程序共享
	// IPC_CREAT：如果与key对应的消息队列不存在则创建它
	// IPC_EXCL | IPC_CREAT：确保这个消息队列是新创建的，否则创建失败
	int msqid = msgget(key, IPC_CREAT | 0660);
	if(msqid == -1)
	{
		perror("Error");
		return;
	}
	printf("msqid: %d\n", msqid);

	/*
	// 不能这样创建消息队列，这可能和已存在的消息队列的key有冲突
	msqid = msgget(123, IPC_CREAT | 0660);
	if(msqid == -1)
	{
		perror("Error");
		return;
	}
	printf("msqid: %d\n", msqid);
	*/

}

