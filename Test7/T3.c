#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SEM_NUM 5

void TestSem();
void TestPV(int semid);
void TestSemCtl(int semid);
void printSemInfo(struct semid_ds * stSem);

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

	char str[20];
	while(1)
	{
		printf("-----------------------------\n");
		printf("输入操作：\n");
		printf("pv: PV操作\n");
		printf("ctl: 控制信号量\n");
		printf("exit: 退出\n");
		fprintf(stderr, "->");
		scanf("%s", str);
		printf("-----------------------------\n");

		if(strcmp(str, "pv") == 0)
		{
			TestPV(semId);
		}
		else if(strcmp(str, "ctl") == 0)
		{
			TestSemCtl(semId);
		}
		else if(strcmp(str, "exit") == 0)
		{
			break;
		}
	}

	printf("Delete Sem(%d)(y/n)? ", semId);
	scanf("%s", str);
	if(strcmp(str, "y") == 0)
	{
		// 删除信号集
		if(semctl(semId, SEM_NUM, IPC_RMID, NULL))
			perror("Error");
		else
			printf("\nDone.\n");
	}

}

void TestPV(int semid)
{
	/*
	// 操作一个信号量
	struct sembuf spos;
	spos.sem_num = 0;	// 设置指定索引的信号量

	// 对指定索引的信号量申请指定数量的资源
	//spos.sem_op = -2;	// <0：P操作，获取资源
	//spos.sem_op = 0;	// =0：Z操作，指定信号量不为0则阻塞，直到信号量为0
	spos.sem_op = +2;	// >0：V操作，释放资源

	spos.sem_flg = 0;

	semop(semid, &spos, 1);
	*/

	// 操作多个信号量
	// P操作：只要有一个资源不够则阻塞
	struct sembuf semArr[SEM_NUM];
	int val, ret;
	while(1)
	{
		for(int i = 0; i < SEM_NUM; i++)
		{
			printf("value[%d]: ", i);
			scanf("%d", &val);
			semArr[i].sem_num = i;
			semArr[i].sem_op = (short)val;


			//semArr[i].sem_flg = 0;		// 默认值，资源不够则阻塞
			semArr[i].sem_flg = SEM_UNDO;	// 如果程序异常退出，申请的资源会自动释放

			// 第一个信号量如果资源不够，立即返回
			// 其它信号量如果资源不够，则阻塞
			if(i == 0)
				semArr[i].sem_flg = IPC_NOWAIT;		// 不阻塞，资源不够立即返回

		}
		ret = semop(semid, semArr, SEM_NUM);
		if(ret)
			perror("Error");
		else
			printf("\nDone.\n");
	}

}

void TestSemCtl(int semid)
{
	unsigned short arr[SEM_NUM] = { 0 };
	int ret;

	while(1)
	{
		printf("输入操作：\n");
		printf("setall: 初始化信号集\n");
		printf("getall: 输出信号集\n");
		printf("stat: 输出信号集信息\n");
		printf("set: 设置信号集权限\n");
		printf("exit: 退出\n");

		char str[20];
		fprintf(stderr, "->");
		scanf("%s", str);

		if(strcmp(str, "setall") == 0)
		{
			int i, val;
			for(i = 0; i < SEM_NUM; i++)
			{
				printf("value[%d]: ", i);
				scanf("%d", &val);
				if(val < 0 || val > 0xffff)
				{
					printf("Invalid value\n");
					break;
				}
				arr[i] = (unsigned short)val;
			}
			if(i != SEM_NUM)
				continue;

			// 信号量的设置
			// SETALL：将数组的值设置到信号集的值，参数2为信号数量，数组类型只能为ushort
			ret = semctl(semid, SEM_NUM, SETALL, arr);
			if(ret)
			{
				perror("Error");
				continue;
			}
		}
		else if(strcmp(str, "getall") == 0)
		{
			// 获取信号量的值
			ret = semctl(semid, SEM_NUM, GETALL, arr);
			if(ret)
			{
				perror("Error");
				continue;
			}
			
			printf("value: ");
			for(int i = 0; i < SEM_NUM; i++)
			{
				printf("%-4d", arr[i]);
			}
			printf("\n");

		}
		else if(strcmp(str, "stat") == 0)
		{
			struct semid_ds stSem;

			// 输出信号集信息
			ret = semctl(semid, SEM_NUM, IPC_STAT, &stSem);
			if(ret)
			{
				perror("Error");
				continue;
			}
			printSemInfo(&stSem);

		}
		else if(strcmp(str, "set") == 0)
		{
			struct semid_ds stSem;
			int new;

			// 输出信号集信息
			ret = semctl(semid, SEM_NUM, IPC_STAT, &stSem);
			if(ret)
			{
				perror("Error");
				continue;
			}

			printf("Current Mode: %#o\n", (int)stSem.sem_perm.mode);
			printf("New Mode: ");
			scanf("%o", &new);
			if(new < 0 || new > 0777)
			{
				fprintf(stderr, "Invalid value\n");
				continue;
			}

			stSem.sem_perm.mode = new;
			ret = semctl(semid, SEM_NUM, IPC_SET, &stSem);
			if(ret)
			{
				perror("Error");
				continue;
			}
			printf("\nDone.\n");

		}
		else if(strcmp(str, "exit") == 0)
		{
			break;
		}

		printf("-----------------------------\n");
	}

}

void printSemInfo(struct semid_ds * stSem)
{
	printf("\n基本信息\n");
	printf("Key: %#x\n", stSem->sem_perm.__key);
	printf("Owner uid: %d\n", stSem->sem_perm.uid);
	printf("Owner guid: %d\n", stSem->sem_perm.gid);
	printf("Creator uid: %d\n", stSem->sem_perm.cuid);
	printf("Creator guid: %d\n", stSem->sem_perm.cgid);
	printf("Mode: %#o\n", stSem->sem_perm.mode);
	printf("Seq: %d\n\n", stSem->sem_perm.__seq);

	printf("操作信息\n");
	printf("Last PV time: %d\n", (int)stSem->sem_otime);
	printf("Last CTRL time: %d\n", (int)stSem->sem_ctime);
	printf("Sem Count: %d\n", (int)stSem->sem_nsems);

}


