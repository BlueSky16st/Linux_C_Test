#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SEM_NUM 5

void TestSem();

int main(void)
{
	TestSem();

    return 0;
}

//创建新的信号集
void TestSem()
{
	char szFile[] = "a.txt";
	key_t key = ftok(szFile, 1);
	if(key == -1)
	{
		perror("Error");
		return;
	}
	printf("Key: %#x\n", key);

	// 设置信号集拥有SEM_NUM个信号量
	int semId = semget(key, SEM_NUM, IPC_CREAT | 0660);
	if(semId < 0)
	{
		perror("Error");
		return;
	}
	printf("Sem 1: %d\n", semId);


	/*

	// 创建新的信号集
	// IPC_EXCL：如果信号集已存在，则出错
	//semId = semget(key, SEM_NUM, IPC_CREAT | IPC_EXCL | 0666);

	// 获取信号集，权限与信号集创建时不一样，新的权限信息是无效的
	//semId = semget(key, SEM_NUM, IPC_CREAT | 0666);

	// 获取信号集，信号量个数与信号集创建时要少，新的信号量个数是无效的
	//semId = semget(key, SEM_NUM - 1, IPC_CREAT | 0666);

	// 获取信号集，信号量个数与信号集创建时要多，则出错
	//semId = semget(key, SEM_NUM + 1, IPC_CREAT | 0666);

	if(semId < 0)
	{
		perror("Error");
		return;
	}
	printf("Sem 2: %d\n", semId);

	*/

	unsigned short arr[] = { 5, 5, 5, 5, 5 };
	// 信号量的设置
	// SETALL：将数组的值设置到信号集的值，参数2为信号集的数量
	int ret = semctl(semId, SET_NUM, SETALL, arr);
	if(ret)
	{
		perror("Error");
		return;
	}
	printf("Init arr to 5 5 5 5 5\n");

}


