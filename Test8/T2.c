#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


void Test();
void Test_Mutex1();
void Test_Mutex2();

int main(void)
{
    //Test();

    //Test_Mutex1();

    Test_Mutex2();

    return 0;
}


// 不使用互斥锁
void * Fun1(void * param)
{
    int * p = (int *)param;
    int n = 10;

    for(int i = 0; i < n; i++)
    {
        *p += i;
        usleep(1000);
    }

    printf("Fun1 param: %d\n", *(int *)param);

    return NULL;

}

void * Fun2(void * param)
{
    int n = *(int *)param;

    usleep(10000);

    printf("Fun2 param: %d\n", n);

    *(int *)param = 0;

    return NULL;

}


void Test()
{
    pthread_t tid1, tid2;
    int x = 0;

    pthread_create(&tid1, NULL, Fun1, (void *)&x);
    pthread_create(&tid2, NULL, Fun2, (void *)&x);

    sleep(1);
    printf("%d\n", x);

}


// 使用互斥锁
pthread_mutex_t lock;

void * FunLock1(void * param)
{
    pthread_mutex_lock(&lock);

    int * p = (int *)param;
    
    while(*p)
    {
        if(!(*p & 1))
        {
            printf("FunLock1: 偶数：%d\n", *p);
        }
        (*p)--;
        usleep(500);
    }
    *p = 10;

    pthread_mutex_unlock(&lock);

    return NULL;

}

void * FunLock2(void * param)
{
    pthread_mutex_lock(&lock);

    int * p = (int *)param;

    while(*p)
    {
        if(*p & 1)
        {
            printf("FunLock2: 奇数：%d\n", *p);
        }
        (*p)--;
    }
    *p = 10;

    pthread_mutex_unlock(&lock);

    return NULL;

}

void Test_Mutex1()
{
    // 创建默认的互斥锁
    pthread_mutex_init(&lock, NULL);

    int x = 10;
    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, FunLock1, (void *)&x);
    pthread_create(&tid2, NULL, FunLock2, (void *)&x);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

}



// 默认锁、检错锁、嵌套锁
int g_x;

void * test1()
{
    printf("test1: %d\n", g_x);

    int ret = pthread_mutex_lock(&lock);
    if(ret)
    {
        printf("test1: 重复锁定\n");
        return NULL;
    }

    for(int i = 0; i < g_x; i++)
    {
        usleep(10000);
        printf("test1: %d\n", i);
    }

    pthread_mutex_unlock(&lock);

    return NULL;
}

void * test2()
{
    int ret = pthread_mutex_lock(&lock);
    if(ret)
    {
        printf("test2: 重复锁定\n");
        return NULL;
    }

    g_x = 10;
    for(int i = 0; i < g_x; i++)
    {
        usleep(10000);
        printf("tes2: %d\n", i);
    }

    // 调用test1()，test1()中也会调用lock()，导致线程阻塞
    test1();

    pthread_mutex_unlock(&lock);

    return NULL;

}

void * test3()
{
    int ret = pthread_mutex_lock(&lock);
    if(ret)
    {
        printf("test3: 重复锁定\n");
        return NULL;
    }

    g_x = 5;
    for(int i = 0; i < g_x; i++)
    {
        usleep(10000);
        printf("test3: %d\n", i);
    }

    pthread_mutex_unlock(&lock);

    return NULL;

}

void Test_Mutex2()
{
    pthread_mutexattr_t attr;

    // 默认锁，可能导致死锁
    //pthread_mutex_init(&lock, NULL);

    // 默认锁，等价于pthread_mutex_init(&lock, NULL)
    //pthread_mutex_t lock_x = PTHREAD_MUTEX_INITIALIZER;

    // 使用检错锁，防止死锁
    //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    //pthread_mutex_init(&lock, &attr);

    // 嵌套锁，允许同一线程多次锁定而不产生不死锁
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock, &attr);



    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, test2, NULL);
    pthread_create(&tid2, NULL, test3, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

}



