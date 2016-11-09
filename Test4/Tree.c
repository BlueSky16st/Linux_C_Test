#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

int cur_dir = 0;

void readDir(DIR * dir);
int isDir(char * fileName);
void printSymbol();

int main(int argc, char * argv[])
{
	char path[128];
	
	if(NULL == getcwd(path, 128))
	{
		perror("Fail to getcwd");
		return -1;
	}
	
	if(argc != 1)
		strcpy(path, argv[1]);
	
	chdir(path);
	getcwd(path, 128);
	DIR * dir = opendir(path);
	if(dir == NULL)
	{
		perror("Fail to opendir");
		return -2;
	}
	
	printf(".\n");
	cur_dir++;
	readDir(dir);
	
	printf("\n");
	
	return 0;
}

void readDir(DIR * dir)
{
	struct dirent * d;
	
	char path[128];
	getcwd(path, 128);
	
	while(d = readdir(dir))
	{
		if(strcmp(d->d_name, "..") == 0
			|| strcmp(d->d_name, ".") == 0)
			continue;
		
		printSymbol();
		printf("%s\n", d->d_name);
		
		if(isDir(d->d_name))
		{
			char nowPath[128];
			chdir(d->d_name);
			if(NULL == getcwd(nowPath, 128))
				continue;
			
			cur_dir++;
			readDir(opendir(nowPath));
			chdir(path);
		}
	}
	cur_dir--;
}

int isDir(char * fileName)
{
	struct stat stInfo;
	int ret = stat(fileName, &stInfo);
	if(ret != 0)
		return 0;
	
	if((stInfo.st_mode & S_IFMT) == S_IFDIR)
		return 1;
	
	return 0;
}

void printSymbol()
{
	int t = cur_dir;
	
	if(cur_dir != 1)
	{
		printf("│");
		while(--t)
			printf("     ");
		printf("│");
	}
	else
	{
		printf("├─");
	}
}