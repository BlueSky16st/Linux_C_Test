#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// gcc编译时需要加入-lpthread

int ret;
void * thread()
{
    // 获取自己线程ID
    printf("Thread ID %d\n", (int)pthread_self());
    
    int i = 5;
    while(i--)
    {
        printf("thread: %d\n", i);
        usleep(1000);
    }

    // 线程退出
    ret = 100;
    pthread_exit(&ret);
    return NULL;

}


void * threadWithParam(void * param)
{
    int n = *(int *)param;
    int iRet = 0;
    int i = 0;

    for(i = 0; i < n; i++)
    {
        iRet += i;
        fprintf(stderr, "thread[%d]: %d ", n, iRet);
        usleep(100 * 1000);
    }
    printf("\n");
    return NULL;

}


typedef struct tagNumArr
{
    int * arr;
    int size;
} N_ARR;

void * largeSort(void * param)
{
    N_ARR stArr;
    memcpy(&stArr, param, sizeof(N_ARR));

    // sort...__a

    return NULL;

}




// 创建线程，线程函数无参数
void TestThread1()
{
    pthread_t id;
    int ret;
    
    ret = pthread_create(&id, NULL, thread, NULL);
    if(ret)
    {
        perror("Error");
        return;
    }
    printf("线程创建成功！\n");

    int i = 10;
    while(i--)
    {
        printf("main: %d\n", i);
    }

    int * p;
    // 等待子线程结束
    pthread_join(id, (void *)&p);
    printf("退出码：%d\n", *p);

}

// 创建线程，线程函数带参数
void TestThread2()
{
    pthread_t tid1, tid2, tid3;
    int x1 = 20, x2 = 15, x3 = 10, i = 0;

    pthread_create(&tid1, NULL, threadWithParam, (void *)&x1);
    pthread_create(&tid2, NULL, threadWithParam, (void *)&x2);
    pthread_create(&tid3, NULL, threadWithParam, (void *)&x3);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

}


// 创建线程，处理排序
void TestThread3()
{
    int arr[] = { 1, 5, 4, 6, 2 };
    N_ARR stArr;
    stArr.arr = arr;
    stArr.size = sizeof(arr) / sizeof(arr[0]);

    pthread_t id;
    pthread_create(&id, NULL, largeSort, (void *)&arr);

    pthread_join(id, NULL);

}



// 获取线程属性
void * Print_SelfAttr()
{
    pthread_attr_t attr;
    pthread_t id = pthread_self();
    pthread_getattr_np(id, &attr);

    int iDetachState;
    pthread_attr_getdetachstate(&attr, &iDetachState);
    printf("Detach: %s\n", iDetachState == PTHREAD_CREATE_DETACHED
                           ? "Detached"
                           : "Joinable");

    return NULL;

}

// 设置线程属性
void TheadAttr()
{
    pthread_attr_t attr;
    
    // 初始化线程属性
    pthread_attr_init(&attr);
    
    // 设置线程分离状态
    // 设置了PTHREAD_CREATE_DETACHED，使pthread_join()无效
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t id;
    pthread_create(&id, &attr, Print_SelfAttr, NULL);

    // 销毁线程属性
    pthread_attr_destroy(&attr);

    sleep(1);
    pthread_join(id, NULL);

}


int main(void)
{
    //TestThread1();

    //TestThread2();

    TheadAttr();

    return 0;
}