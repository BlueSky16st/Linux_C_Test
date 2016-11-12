// 另一版本，带颜色地显示树形结构
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

/*
\033[显示方式;前景色;背景色m

默认颜色：\033[0m
前景红色背景黑色：\033[0;31;40m
*/

void myTree(char * szPath, int deep);

int main(int argc, char * argv[])
{
	char szPath[PATH_MAX];
	
	if(argc == 2)
		chdir(argv[1]);
	getcwd(szPath, PATH_MAX);
	myTree(szPath, 0);
	
	return 0;
}

void myTree(char * szPath, int deep)
{
	DIR * pDir;
	struct dirent * pDent;
	struct stat stInfo;
	char szSubPath[PATH_MAX];
	
	pDir = opendir(szPath);
	if(pDir == NULL)
	{
		perror("Fail to opendir");
		return;
	}
	
	while(1)
	{
		pDent = readdir(pDir);
		if(pDent == NULL)
			break;
		
		if(strcmp(pDent->d_name, ".") == 0
			|| strcmp(pDent->d_name, "..") == 0)
			continue;
		
		for(int i = 0; i != deep; ++i)
			printf("│   ");
		
		if(pDent->d_type == DT_DIR)
		{
			printf("├─  \033[1;34m%s\033[0m\n", pDent->d_name);
			sprintf(szSubPath, "%s/%s", szPath, pDent->d_name);
			myTree(szSubPath, deep + 1);
		}
		else
		{
			sprintf(szSubPath, "%s/%s", szPath, pDent->d_name);
			stat(szSubPath, &stInfo);
			
			if(S_ISREG(stInfo.st_mode))
			{
				// 可执行文件
				if(stInfo.st_mode & 0100)
					printf("├─  \033[1;32m%s\033[0m\n", pDent->d_name);
				else
					printf("├─  %s\n", pDent->d_name);
			}
			else if(S_ISFIFO(stInfo.st_mode))
			{
				printf("├─  \033[1;33;42m%s\033[0m\n", pDent->d_name);
			}
		}
	}
	closedir(pDir);
}
