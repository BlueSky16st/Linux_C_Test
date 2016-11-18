#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

/*
	关于return、exit、_exit的区别
*/

void fun()
{
	cout << "Fun()" << endl;
}


class A
{
public:
	A()
	{
		cout << "Construct" << endl;
	}
	~A()
	{
		cout << "Destroy" << endl;
	}
};

void TestA()
{
	// 注册一个回调函数，在exit()终止进程前会执行回调函数
	atexit(fun);
	
	cout << "TestA: " << endl;
	
	A a;
	
	cout << "End TestA" << endl;
	
	// 释放句柄、变量，调用局部对象的析构函数、回到上一级函数
	//return;
	
	// 终止进程前，关闭文件，清理I/O，调用atexit()指定的回调函数
	//exit(0);
	
	// 立即终止进程，不关闭文件，不清理I/O
	_exit(0);
	
}

int main(void)
{
	TestA();
	
	cout << "End main" << endl;
	
	return 0;
}