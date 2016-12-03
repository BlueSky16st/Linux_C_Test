#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void TestMsgRecvSend();
void TestMsgCtl();

int main(void)
{
	TestMsgCtl();

	return 0;
}


// 发送消息和接收消息
typedef struct tagMsg
{
	long type;
	char szBuf[1024];
} MSG;

void TestMsgRecvSend()
{
	// s(end)或r(ecv)
	char fl = getchar();

	if(fl != 'r' && fl != 's')
		return;

	key_t key = ftok("a.txt", 1);
	if(key == -1)
	{
		perror("Error");
		return;
	}
	printf("Key: %d\n", key);

	int msqid = msgget(key, IPC_CREAT | 0666);
	if(msqid == -1)
	{
		perror("Error");
		return;
	}
	printf("msqid: %d\n", msqid);

	MSG stMsg = { 0 };

	if(fl == 'r')	// Recv
	{
		while(1)
		{
			memset(&stMsg, 0, sizeof(stMsg));

			fprintf(stderr, "Type: ");
			scanf("%ld", &(stMsg.type));

			// 如果没有对应type类型的消息，阻塞
			int iRet = msgrcv(msqid, &stMsg, sizeof(stMsg), stMsg.type, 0);

			// 如果没有对应type类型的消息，返回-1
			//int iRet = msgrcv(msqid, &stMsg, sizeof(stMsg), stMsg.type, IPC_NOWAIT);

			// 接受除了type类型的所有消息
			// 需要定义宏：#define _GNU_SOURCE 才可以使用MSG_EXCEPT
			//int iRet = msgrcv(msqid, &stMsg, sizeof(stMsg), stMsg.type, MSG_EXCEPT);

			// 如果消息太长，则截断消息
			//int iRet = msgrcv(msqid, &stMsg, sizeof(stMsg), stMsg.type, MSG_NOERROR);

			if(iRet < 0)
			{
				perror("Error");
				break;
			}
			printf("Recv: %s\n", stMsg.szBuf);

		}
	}
	else		// Send
	{
		while(1)
		{
			fprintf(stderr, "Send: ");
			scanf("%ld%s", &(stMsg.type), stMsg.szBuf);

			// 发送消息
			int iRet = msgsnd(msqid, &stMsg, strlen(stMsg.szBuf), 0);
			if(iRet != 0)
			{
				perror("Error");
				break;
			}

		}
	}


}



// 获取消息队列信息
void PrintSTATInfo(struct msqid_ds * pstMsgQ)
{
	printf("\n---------------------------------\n\n");

	printf("权限信息\n");
	printf("key: %d\n", pstMsgQ->msg_perm.__key);
	printf("Ownner uid: %d\n", pstMsgQ->msg_perm.uid);
	printf("Ownner gid: %d\n", pstMsgQ->msg_perm.gid);
	printf("Creator uid: %d\n", pstMsgQ->msg_perm.cuid);
	printf("Creator gid: %d\n", pstMsgQ->msg_perm.cgid);
	printf("Mode: %#o\n", pstMsgQ->msg_perm.mode);
	printf("Seq: %d\n", pstMsgQ->msg_perm.__seq);

	printf("\n---------------------------------\n\n");

	printf("其它信息\n");
	printf("Send Time: %d\n", (int)pstMsgQ->msg_stime);
	printf("Receive Time: %d\n", (int)pstMsgQ->msg_rtime);
	printf("Change Time: %d\n", (int)pstMsgQ->msg_ctime);
	printf("Current Bytes: %d\n", (int)pstMsgQ->__msg_cbytes);
	printf("Message Max Bytes: %d\n", (int)pstMsgQ->msg_qbytes);
	printf("Message Count: %d\n", (int)pstMsgQ->msg_qnum);
	printf("Last Send pid: %d\n", pstMsgQ->msg_lspid);
	printf("Last Receive pid: %d\n", pstMsgQ->msg_lrpid);

	printf("\n---------------------------------\n\n");

}

void PrintfINFOInfo(struct msginfo * pInfo)
{
	printf("\n---------------------------------\n\n");

	printf("系统限制信息\n");
	printf("Message Max Length: %d\n", pInfo->msgmax);
	printf("Message Max Number: %d\n", pInfo->msgmni);
	printf("Message Max Length(Bytes): %d\n", pInfo->msgmnb);

	printf("\n---------------------------------\n\n");	

	printf("当前消息队列信息\n");
	printf("Current Queue Count: %d\n", pInfo->msgpool);
	printf("Current Message Count: %d\n", pInfo->msgmap);
	printf("Current All Messages Length(Bytes): %d\n", pInfo->msgtql);


	printf("\n---------------------------------\n\n");

}

void ShowParamInfo()
{
	printf("\n输入以下参数\n");
	printf("stat: 获取权限信息\n");
	printf("info: 获取系统消息队列信息\n");
	printf("set: 设置权限信息\n");
	printf("exit: 退出程序\n\n");

}

void TestMsgCtl()
{
	key_t key = ftok("a.txt", 1);
	if(key == -1)
	{
		perror("Error");
		return;
	}

	printf("Key: %d\n", key);

	int msqid = msgget(key, IPC_CREAT | 0666);
	if(msqid == -1)
	{
		perror("Error");
		return;
	}
	printf("msqid: %d\n", msqid);

	struct msqid_ds stMsgQ;
	char szCmd[20] = { 0 };
	int ret;
	while(1)
	{
		ShowParamInfo();

		fprintf(stderr, "cmd:");
		scanf("%s", szCmd);

		if(!strcmp(szCmd, "stat"))
		{
			// IPC_STAT:获取属性
			msgctl(msqid, IPC_STAT, &stMsgQ);

			PrintSTATInfo(&stMsgQ);

		}
		else if(!strcmp(szCmd, "info"))
		{
			// MSG_INFO:获取系统消息队列信息
			// 需要定义宏：#define _GNU_SOURCE 才可以使用MSG_INFO
			// 指定MSG_INFO之后,stMsgQ的类型需要转换为msginfo才可以使用
			msgctl(msqid, MSG_INFO, &stMsgQ);

			PrintfINFOInfo((struct msginfo *)&stMsgQ);

		}
		else if(!strcmp(szCmd, "set"))
		{
			msgctl(msqid, IPC_STAT, &stMsgQ);

			int mode;
			printf("Current mode: %#o\n", stMsgQ.msg_perm.mode);
			fprintf(stderr, "Enter new mode: ");
			scanf("%o", &mode);
			if(mode > 0777 || mode < 0)
			{
				printf("Invalid mode\n");
				continue;
			}
			stMsgQ.msg_perm.mode = mode;

			// IPC_SET:设置属性
			ret = msgctl(msqid, IPC_SET, &stMsgQ);
			if(ret)
			{
				perror("Error");
				continue;
			}
			else
			{
				PrintSTATInfo(&stMsgQ);
			}
		}
		else if(!strcmp(szCmd, "exit"))
		{
			break;
		}
		
	}

	fprintf(stderr, "\n\n----End----\n\nDelete message queue?(y/n)");
	scanf("%s", szCmd);
	// IPC_RMID:删除消息队列
	if(!strcmp(szCmd, "y"))
	{
		ret = msgctl(msqid, IPC_RMID, NULL);
		if(ret)
			perror("Error");
		else
			printf("----Done----\n\n");
	}

}


