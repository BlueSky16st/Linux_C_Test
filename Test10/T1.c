#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

void TestMySQL();

int main(void)
{
	TestMySQL();

	return 0;
}

// MySQL数据库
void TestMySQL()
{
	int ret;
	MYSQL conn;
	
	// 初始化
	mysql_init(&conn);

	// 连接
	if(mysql_real_connect(&conn, "localhost", "root", "0127", "MySQLDatabase", 0, NULL, 0) == NULL)
	{
		printf("连接数据库失败：%s\n", mysql_error(&conn));
		return;
	}

	char cmd[10];
	char args[3][20];
	char sql[200];
	while(1)
	{
		printf("\n\n----------------------\n");
		printf("add：增加学生\n");
		printf("del：输入姓名删除学生\n");
		printf("upd：输入姓名修改学生\n");
		printf("sel：查询学生\n");
		printf("exit：退出程序\n");
		printf("----------------------\n\n");
		printf("->");
		scanf("%s", cmd);

		if(strcmp(cmd, "add") == 0)
		{
			printf("输入：姓名 性别 电话\n->");
			scanf("%s%s%s", args[0], args[1], args[2]);

			sprintf(sql, "insert into student(name, sex, mobile) values('%s', '%s', '%s')",
						 args[0], args[1], args[2]);
			

		}
		else if(strcmp(cmd, "del") == 0)
		{
			printf("输入：姓名\n->");
			scanf("%s", args[0]);

			sprintf(sql, "delete from student where name='%s'", args[0]);
			
			
		}
		else if(strcmp(cmd, "upd") == 0)
		{
			printf("输入：姓名 列名 新值\n->");
			scanf("%s%s%s", args[0], args[1], args[2]);

			sprintf(sql, "update  student student set %s='%s' where name='%s'", args[1], args[2], args[0]);


		}
		else if(strcmp(cmd, "sel") == 0)
		{
			printf("输入：姓名或*\n->");
			scanf("%s", args[0]);

			if(!strcmp(args[0], "*"))
			{
				sprintf(sql, "select * from student");
			}
			else
			{
				sprintf(sql, "select * from student where name='%s'", args[0]);
			}


		}
		else if(strcmp(cmd, "exit") == 0)
		{
			break;
		}
		else
		{
			continue;
		}

		ret = mysql_real_query(&conn, sql, strlen(sql));
		if(ret)
		{
			printf("[%s]\n执行失败：%s", sql, mysql_error(&conn));
			continue;
		}

		if(strcmp(cmd, "sel") == 0)
		{
			char fmt[] = "|%4s|%8s|%8s|%11s|\n";
			
			

			MYSQL_RES * res = mysql_use_result(&conn);
			
			MYSQL_ROW row;
			printf("+----+--------+---------+----------+\n");
			printf(fmt, "id", "name", "sex", "mobile");
			while((row = mysql_fetch_row(res)) != NULL)
			{
				printf(fmt, row[0], row[1], row[2], row[3]);
			}
			printf("+----+--------+---------+----------+\n");
		}

		printf("\n执行成功\n");

	}

	mysql_close(&conn);

}



