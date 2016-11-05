#include <stdio.h>
#include <unistd.h>

// stdout设置无缓冲，立即输出
int main(void)
{
	int arr[] = { 1, 2, 3, 4, 5, 6 };
	size_t sz = sizeof(arr) / sizeof(arr[0]);

	setbuf(stdout, NULL);
	for(size_t i = sz - 1; i >= 0; --i)
	{
		usleep(100000);
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;

}
