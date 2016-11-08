#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// frepoen()������ʵ���ض���
void Test_freopen()
{
	FILE * pFile;
	
	fprintf(stderr, "This is line1\n");
	
	// ��stderr�ض����ļ�"1.txt"
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
	
	// �ļ��ر��ˣ��޷������һ�С�
	fprintf(stderr, "This is end line 1\n");
	printf("This is end line 2\n");
	
	/////////////////////////////////////////////////////////
	
	char szBuf[100];
	scanf("%s", szBuf);
	printf("Your input is %s\n", szBuf);
	
	// ��stdin�ض����ļ�"1.txt"
	pFile = freopen("1.txt", "r", stdin);
	if(pFile == NULL)
	{
		printf("File to open\n");
		return;
	}
	// scanf("%s", szBuf);	// ���ļ��ж�ȡ�ַ�ֱ�������ո�
	scanf("%[^\n]", szBuf);	// ʹ��������ʽ��ȡһ��
	printf("Your input is %s\n", szBuf);
	
	fclose(pFile);
	
}

// EOFΪint���͵�-1����ȡ�ļ�ʱ�����ܽ�����char����ֵ���Ƚ�
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
	
	// ����ģ����ʱֻ�����szBuf�е�2�С�
	/*
	char c;
	while((c = fgetc(pFile)) != EOF)	// char���͵�-1Ϊ255����EOF��ȡ�
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