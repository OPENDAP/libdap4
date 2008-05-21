/*#include "apue.h"*/
#include "apue_db.h"
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
 
void	err_quit(const char *, ...);
void	err_sys(const char *, ...);

/*
 * Default file access permissions for new files.
 */
#define FILE_MODE       (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int
main(void)
{
	DBHANDLE	db;

	if ((db = db_open("db4", O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) == NULL)
		err_sys("db_open error");

	if (db_store(db, "Alpha", "data1", DB_INSERT) != 0)
		err_quit("db_store error for alpha");
	if (db_store(db, "beta", "Data for beta", DB_INSERT) != 0)
		err_quit("db_store error for beta");
	if (db_store(db, "gamma", "record3", DB_INSERT) != 0)
		err_quit("db_store error for gamma");

	db_close(db);
	exit(0);
}
