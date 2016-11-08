#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// frepoen()函数，实现重定向
void Test_freopen()
{
	FILE * pFile;
	
	fprintf(stderr, "This is line1\n");
	
	// 将stderr重定向到文件"1.txt"
	pFile = freopen("1.txt", "w", stderr);
	if(pFile == NULL)
	{
		printf("Fail to open\n");
		return;
	}
	
	fprintf(stderr, "This is line2\n");
	printf("This is line3\n");
	
	fprintf(pFile, "This is line for pFile\n");
	
	fclose(pFile);
	
	// 文件关闭了，无法输出这一行。
	fprintf(stderr, "This is end line 1\n");
	printf("This is end line 2\n");
	
	/////////////////////////////////////////////////////////
	
	char szBuf[100];
	scanf("%s", szBuf);
	printf("Your input is %s\n", szBuf);
	
	// 将stdin重定向到文件"1.txt"
	pFile = freopen("1.txt", "r", stdin);
	if(pFile == NULL)
	{
		printf("File to open\n");
		return;
	}
	// scanf("%s", szBuf);	// 从文件中读取字符直到遇到空格。
	scanf("%[^\n]", szBuf);	// 使用正则表达式读取一行
	printf("Your input is %s\n", szBuf);
	
	fclose(pFile);
	
}

// EOF为int类型的-1，读取文件时，不能将它与char类型值做比较
void ReadFile()
{
	FILE * pFile = fopen("2.txt", "w");
	char szBuf[100];
	sprintf(szBuf, "This is line1.\nThis is line2%c.\nThis is line3\n", 255);
	fprintf(pFile, szBuf);
	fclose(pFile);
	
	
	pFile = fopen("2.txt", "r");
	if(pFile == NULL)
	{
		printf("Fail to open\n");
		return;
	}
	
	// 错误的，输出时只输出了szBuf中的2行。
	/*
	char c;
	while((c = fgetc(pFile)) != EOF)	// char类型的-1为255，与EOF相等。
	{
		printf("%c", c);
	}
	*/
	
	int c;
	while((c = fgetc(pFile)) != EOF)
	{
		printf("%c", (char)c);
	}
	
	fclose(pFile);
	
}

int main(void)
{
	//Test_freopen();
	
	ReadFile();
	
	return 0;
}