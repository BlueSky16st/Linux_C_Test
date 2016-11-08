#include <stdio.h>
#include <sys/stat.h>

// 获取Linux文件属性和权限信息
int main(int argc, char * argv[])
{
	if(argc == 1)
		return 0;

	struct stat stInfo;
	
	int ret = stat(argv[1], &stInfo);

	if(ret != 0)
	{
		printf("Fail to stat\n");
		return 0;
	}

	switch(stInfo.st_mode & S_IFMT)
	{
	case S_IFREG:
	{
		printf("Regular file!\n");
		break;
	}
	case S_IFDIR:
	{
		printf("Directory file!\n");
		break;
	}
	case S_IFCHR:
	{
		printf("Character file!\n");
		break;
	}
	case S_IFBLK:
	{
		printf("Block file!\n");
		break;
	}
	case S_IFIFO:
	{
		printf("FIFO file!\n");
		break;
	}
	case S_IFLNK:
	{
		printf("Link file!\n");
		break;
	}
	case S_IFSOCK:
	{
		printf("Sock file!\n");
		break;
	}
	default:
	{
		printf("Unknown file!\n");
		break;
	}

	}

	int authsMask[] = {0400, 0200, 0100, 0040, 0020, 0010, 0004, 0002, 0001};
	char authChars[] = { 'r', 'w', 'x' };
	int i;
	for(int i = 0; i < sizeof(authsMask) / sizeof(authsMask[0]); ++i)
	{
		if(stInfo.st_mode & authsMask[i])
		{
			printf("%c", authChars[i % 3]);
		}
		else
		{
			printf("-");
		}
	}
	printf("\n");

	return 0;
}
