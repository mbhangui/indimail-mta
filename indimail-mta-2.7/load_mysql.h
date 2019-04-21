/*
 * $Log: load_mysql.h,v $
 * Revision 1.2  2019-04-21 10:28:17+05:30  Cprogrammer
 * include mysql.h for MYSQL definition
 *
 * Revision 1.1  2019-04-20 19:48:09+05:30  Cprogrammer
 * Initial revision
 *
 */
#ifndef LOAD_MYSQL_H
#define LOAD_MYSQL_H
#include <mysql.h>

typedef struct MYSQL_RES res;
typedef unsigned int i_uint;
extern int      use_sql;
extern MYSQL   *(*in_mysql_init) (MYSQL *);
extern MYSQL   *(*in_mysql_real_connect) (MYSQL *, const char *, const char *, const char *, const char *, unsigned int, const char *, unsigned long);
const char     *(*in_mysql_error) (MYSQL *);
extern i_uint   (*in_mysql_errno) (MYSQL *mysql);
extern void     (*in_mysql_close) (MYSQL *);
extern int      (*in_mysql_options) (MYSQL *, enum mysql_option, const void *);
extern int      (*in_mysql_query) (MYSQL *, const char *);
extern res     *(*in_mysql_store_result) (MYSQL *);
extern char   **(*in_mysql_fetch_row) (MYSQL_RES *);
extern my_ulonglong (*in_mysql_num_rows)(MYSQL_RES *);
extern my_ulonglong (*in_mysql_affected_rows) (MYSQL *);
extern void     (*in_mysql_free_result) (MYSQL_RES *);

int             initMySQLlibrary(char **);

#endif
