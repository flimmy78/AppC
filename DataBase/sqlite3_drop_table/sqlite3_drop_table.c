#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
/*************************
sqlite3_open:打开数据库，如果数据库不存在，就创建一个新的数据库





**************************/
sqlite3 * SqliteOpen(sqlite3 *db)
{
    int result;
 
    //鎵撳紑鎸囧畾鐨勬暟鎹簱鏂囦欢,濡傛灉涓嶅瓨鍦ㄥ皢鍒涘缓涓�涓悓鍚嶇殑鏁版嵁搴撴枃浠�
    result = sqlite3_open("test.db", &db);
    if(result != SQLITE_OK)
    {
   	    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	    sqlite3_close(db);
	    return NULL;
     }
     else 
     {
    	     printf("Open Database test.db success!\n");
     }
     return db;
}
/*************************
sqlite3_exec:执行数据库操作命令
create table:创建表




**************************/
int SqliteCreateTbl(sqlite3 *db)
{
       int result;
	char *errmsg = 0;
	char *SqlCmd ="create table Table1(id int, name int,addr int)";
	result=sqlite3_exec( db, SqlCmd , NULL, NULL, &errmsg); 
	if( result != SQLITE_OK ) 
	{
		 fprintf(stderr, "Create table database error: %s\n", sqlite3_errmsg(db));
		//关闭数据库 
		sqlite3_close(db); 
		return -1;
	}
	else
	{
 		printf("Create Database table success!\n");
	}
	sqlite3_close(db); 
	return 0;
}

/*************************
sqlite3_exec:执行数据库操作命令
create table:创建表




**************************/
int SqliteDropTbl(sqlite3 *db)
{
       int result;
	char *errmsg = 0;
	char *SqlCmd ="drop table Table1";
	result=sqlite3_exec( db, SqlCmd , NULL, NULL, &errmsg); 
	if( result != SQLITE_OK ) 
	{
		 fprintf(stderr, "Drop table  error: %s\n", sqlite3_errmsg(db));
		//关闭数据库 
		sqlite3_close(db); 
		return -1;
	}
	else
	{
 		printf("Drop database table success!\n");
	}
	sqlite3_close(db); 
	return 0;


}

int main( void )
{
    sqlite3 *dbOpen=NULL;
    sqlite3 *db = NULL;
    db = SqliteOpen(dbOpen);
    SqliteCreateTbl(db);
    //SqliteDropTbl(db);
}

