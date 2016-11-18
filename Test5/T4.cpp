#include <iostream>
#include <string>

using namespace std;

void Print(string str)
{
	str[0] = 'h';
}

// C++中的string类的写时拷贝
int main(void)
{
	string str1 = "Hello World";
	string str2 = str1;
	
	// 地址是一样的
	printf("str1: %p\n", str1.c_str());
	printf("str2: %p\n", str2.c_str());
	
	//str1[0] = 'h';
	Print(str1);
	
	// 地址不同了，只要有修改的动作，则复制对象
	printf("str1: %p\n", str1.c_str());
	printf("str2: %p\n", str2.c_str());
	
	return 0;
	
}