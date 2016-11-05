#include <stdio.h>

int * add(int * x)
{
	*x = *x + 152;
	return x;

}

// 运行时程序出错
int main(void)
{
	int * a = NULL;
	add(a);
	printf("OK\n");

	return 0;
}
