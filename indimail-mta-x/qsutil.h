/*
 * $Log: qsutil.h,v $
 * Revision 1.18  2021-06-27 11:33:49+05:30  Cprogrammer
 * added loglock_open function
 *
 * Revision 1.17  2021-06-23 13:23:03+05:30  Cprogrammer
 * include sys/types.h for size_t
 *
 * Revision 1.16  2021-06-23 10:04:01+05:30  Cprogrammer
 * added log_stat function
 *
 * Revision 1.15  2021-06-05 12:53:00+05:30  Cprogrammer
 * added log4_noflush() function
 *
 * Revision 1.14  2021-06-04 09:22:09+05:30  Cprogrammer
 * added log15() function
 *
 * Revision 1.13  2021-05-30 00:14:28+05:30  Cprogrammer
 * added log11() function
 *
 * Revision 1.12  2021-05-08 12:25:15+05:30  Cprogrammer
 * include stralloc.h
 *
 * Revision 1.11  2016-03-31 17:02:49+05:30  Cprogrammer
 * added log3_noflush(), lockerr(), flush() functions
 *
 * Revision 1.10  2016-01-29 18:30:38+05:30  Cprogrammer
 * removed log11() and added log13()
 *
 * Revision 1.9  2014-03-07 19:15:11+05:30  Cprogrammer
 * added log9(), log11()
 *
 * Revision 1.8  2014-02-05 01:05:08+05:30  Cprogrammer
 * added prototype for log7()
 *
 * Revision 1.7  2013-09-23 22:13:25+05:30  Cprogrammer
 * added log functions which do not flush
 *
 * Revision 1.6  2009-05-03 22:46:56+05:30  Cprogrammer
 * added log5() function
 *
 * Revision 1.5  2004-12-20 22:58:09+05:30  Cprogrammer
 * changed log2() to my_log2() to avoid conflicts in fedora3
 *
 * Revision 1.4  2004-10-11 14:00:03+05:30  Cprogrammer
 * added function prototypes
 *
 * Revision 1.3  2004-06-18 23:01:30+05:30  Cprogrammer
 * added RCS log
 *
 */
#ifndef QSUTIL_H
#define QSUTIL_H
#include <stralloc.h>
#include <sys/types.h>

void            loglock_open(int);
void            lockerr(void);
void            log1(char *);
void            log1_noflush(char *);
void            my_log2(char *, char *);
void            log2_noflush(char *, char *);
void            log3_noflush(char *, char *, char *);
void            log4_noflush(char *, char *, char *, char *);
void            log3(char *, char *, char *);
void            log4(char *, char *, char *, char *);
void            log5(char *, char *, char *, char *, char *);
void            log7(char *, char *, char *, char *, char *, char *, char *);
void            log9(char *, char *, char *, char *, char *, char *, char *, char *, char *);
void            log11(char *, char *, char *, char *, char *, char *, char *, char *, char *, char *, char *);
void            log13(char *, char *, char *, char *, char *, char *, char *, char *, char *, char *, char *, char *, char *);
void            log15(char *, char *, char *, char *, char *, char *, char *, char *, char *, char *, char *, char *, char *, char *, char *);
void            logsa(stralloc *);
void            logsa_noflush(stralloc *);
void            log_stat(stralloc *, stralloc *, unsigned long, size_t);
void            nomem(void);
void            pausedir(char *);
void            logsafe(char *);
void            logsafe_noflush(char *);
void            flush();

extern int      loglock_fd;

#endif
