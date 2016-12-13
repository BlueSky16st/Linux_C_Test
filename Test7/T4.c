#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define FILENAME	"b.txt"
#define SHMSIZE		0x100000	//1MB

void WriteMemory(int shmid);
void ReadMemory(int shmid);
void ControlMemory(int shmid);
void PrintfShmLimit(struct shminfo * pInfo);

void PrintShmInfo(struct shmid_ds * pInfo);
void PrintShmUsed(struct shm_info * pInfo);

int main(int argc, char ** argv)
{
    if(argc != 2 || (!strcmp(argv[1], "r") 
                    && !strcmp(argv[1], "w")
                    && !strcmp(argv[1], "o")))
    {
        printf("读内存：r\n写内存：w\n其它：o\n");
        return 0;
    }

    key_t key = ftok(FILENAME, 1);
    if(key < 0)
    {
        perror("Error");
        return -1;
    }
    printf("key: %#x\n", key);


    // 创建共享内存
    int shmid = shmget(key, SHMSIZE, IPC_CREAT | 0600);
    if(shmid < 0)
    {
        perror("Error");
        return -2;
    }
    printf("shmid: %d\n", shmid);


    if(argv[1][0] == 'r')
    {
        ReadMemory(shmid);
    }
    else if(argv[1][0] == 'w')
    {
        WriteMemory(shmid);
    }
    else if(argv[1][0] == 'o')
    {
        ControlMemory(shmid);
    }

    return 0;
}

typedef struct tagStudent
{
    char szName[20];
    int age;
    char szTel[16];
} STUDENT;

// 写内存
void WriteMemory(int shmid)
{
    STUDENT * pStu;
    // 获取内存数据
    pStu = (STUDENT *)shmat(shmid, NULL, 0);
    // 自定义内存位置，如果指定的位置已被使用，则出错
    //pStu = (STUDENT *)shmat(shmid, (void *)0x12345678, SHM_RND);
    if(pStu == (STUDENT *)-1)
    {
        perror("Error");
        return;
    }

    int index;
    STUDENT stu;
    while(1)
    {
        memset(&stu, 0, sizeof(stu));

        printf("写入 - 输入位置：");
        scanf("%d", &index);
        if(index < 0 || index >= SHMSIZE % sizeof(stu))
        {
            printf("Error\n");
            continue;
        }

        printf("输入姓名、年龄、电话（以空格隔开）：");
        scanf("%s%d%s", stu.szName, &stu.age, stu.szTel);

        memcpy(pStu + index, &stu, sizeof(stu));
    }

    shmdt(pStu);

}

// 读内存
void ReadMemory(int shmid)
{
    STUDENT * pStu;
    // 获取内存数据
    pStu = (STUDENT *)shmat(shmid, NULL, 0);
    if(pStu == NULL)
    {
        perror("Error");
        return;
    }

    int index;
    STUDENT stu;
    while(1)
    {
        memset(&stu, 0, sizeof(stu));

        printf("读取 - 输入位置：");
        scanf("%d", &index);
        if(index < 0 || index >= SHMSIZE % sizeof(stu))
        {
            printf("Error\n");
            continue;
        }

        memcpy(&stu, pStu + index, sizeof(stu));
        printf("名字：%s\n", stu.szName);
        printf("年龄：%d\n", stu.age);
        printf("电话：%s\n", stu.szTel);
    }

    shmdt(pStu);

}

void ControlMemory(int shmid)
{
    struct shmid_ds stShm;
    char szCmd[50];
    int ret;

    while(1)
    {
        printf("stat: 显示共享内存信息\n");
        printf("ipcinfo: 显示共享内存限制信息\n");
        printf("shminfo: 显示共享使用信息\n");
        printf("exit: 退出\n");

        printf("->");
        scanf("%s", szCmd);

        if(!strcmp(szCmd, "stat"))
        {
            ret = shmctl(shmid, IPC_STAT, &stShm);
            if(ret)
            {
                perror("Error");
                continue;
            }
            PrintShmInfo(&stShm);
        }
        else if(!strcmp(szCmd, "ipcinfo"))
        {
            // 第三个参数为shm_info结构体，使用时需要强制转换
            ret = shmctl(shmid, IPC_INFO, &stShm);
            if(ret == -1)
            {
                perror("Error");
                continue;
            }
            PrintfShmLimit((struct shminfo *)&stShm);

        }
        else if(!strcmp(szCmd, "shminfo"))
        {
            ret = shmctl(shmid, SHM_INFO, &stShm);
            if(ret == -1)
            {
                perror("Error");
                continue;
            }
            PrintShmUsed((struct shm_info *)&stShm);
        }
        //else if(!strcmp(szCmd, ""))
        else if(!strcmp(szCmd, "exit"))
        {
            break;
        }

    }

    printf("删除共享内存(y/n)？");
    scanf("%s", szCmd);
    if(!strcmp(szCmd, "y"))
    {
        ret = shmctl(shmid, IPC_RMID, NULL);
        if(ret)
            perror("Error");
        else
            printf("Done.\n");
    }

}

void PrintShmInfo(struct shmid_ds * pInfo)
{
    printf("共享内存权限信息\n");
    printf("key: %#x\n", pInfo->shm_perm.__key);
    printf("Owner uid: %d\n", (int)pInfo->shm_perm.uid);
    printf("Owner gid: %d\n", (int)pInfo->shm_perm.gid);
    printf("Mode: %#o\n", pInfo->shm_perm.mode);
    printf("Seq: %d\n", pInfo->shm_perm.__seq);

    printf("\n");

    printf("共享内存基本信息\n");
    printf("大小：%#x\n", (int)pInfo->shm_segsz);
    printf("进程连接数： %d\n", (int)pInfo->shm_nattch);
    printf("最后一次连接时间： %d\n", (int)pInfo->shm_atime);
    printf("最后一次断开连接时间： %d\n", (int)pInfo->shm_dtime);
    printf("最后一次修改时间： %d\n", (int)pInfo->shm_ctime);
    printf("创建者的pid： %d\n", (int)pInfo->shm_cpid);
    printf("最后一次修改的pid： %d\n", (int)pInfo->shm_lpid);

}

void PrintfShmLimit(struct shminfo * pInfo)
{
    printf("共享内存限制信息\n");
    printf("共享内存允许最大段数： %#x\n", (unsigned int)pInfo->shmmax);
    printf("共享内存允许最小段数： %#x\n", (unsigned int)pInfo->shmmin);
    printf("共享内存最大个数： %#x\n", (unsigned int)pInfo->shmmni);
    printf("共享内存最大页数： %#x\n", (unsigned int)pInfo->shmall);

}

void PrintShmUsed(struct shm_info * pInfo)
{
    printf("共享内存使用信息\n");
    printf("当前共享内存数： %d\n", (int)pInfo->used_ids);
    printf("当前共享内存页： %d\n", (int)pInfo->shm_tot);
    printf("驻留内存页： %d\n", (int)pInfo->shm_rss);
    printf("交换区内存页： %d\n", (int)pInfo->shm_swp);
    
}

