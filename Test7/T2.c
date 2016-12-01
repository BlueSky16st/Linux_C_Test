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



// 获取消息队列信息并打印
void PrintMsqInfo(struct msqid_ds * pstMsgQ)
{
	printf("\n\n权限信息\n");
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

	printf("\n---------------------------------\n");

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
	msgctl(msqid, IPC_STAT, &stMsgQ);

	PrintMsqInfo(&stMsgQ);



}


