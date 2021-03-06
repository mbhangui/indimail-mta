/*
 * $Log: cdb-database.c,v $
 * Revision 1.1  2021-06-15 11:30:39+05:30  Cprogrammer
 * Initial revision
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stralloc.h>
#include <subfd.h>
#include <getln.h>
#include <substdio.h>
#include <cdbmss.h>
#include <byte.h>
#include <env.h>
#include <strerr.h>
#include <open.h>
#include <error.h>
#include <case.h>
#include "auto_control.h"
#include "variables.h"

#define FATAL "cdb-database: fatal: "

int             rename(const char *, const char *);

struct cdbmss   cdbmss;
stralloc        key = { 0 };
stralloc        data = { 0 };
stralloc        line = { 0 };
stralloc        wildchars = { 0 };
stralloc        fntmp = {0};
stralloc        fncdb = {0};
char            inbuf[1024];
substdio        ssin;

int
main(int argc, char **argv)
{
	int             i, fd, fdtemp, match;
	int             numcolons;

	if (argc != 2)
		strerr_die1x(100, "usage: cdb-database filename");
	controldir = (controldir = env_get("CONTROLDIR")) ? controldir : auto_control;
	if (chdir(controldir) == -1)
		strerr_die4sys(111, FATAL, "chdir: ", controldir, ": ");
	if ((fd = open_read(argv[1])) == -1)
		strerr_die3sys(111, FATAL, argv[1], ": ");
	substdio_fdbuf(&ssin, read, fd, inbuf, sizeof(inbuf));
	if (!stralloc_copys(&fntmp, argv[1]) || !stralloc_catb(&fntmp, ".tmp", 4) || !stralloc_0(&fntmp))
		strerr_die2sys(111, FATAL, "out of memory");
	if (!stralloc_copys(&fncdb, argv[1]) || !stralloc_catb(&fncdb, ".cdb", 4) || !stralloc_0(&fncdb))
		strerr_die2sys(111, FATAL, "out of memory");
	if ((fdtemp = open_trunc(fntmp.s)) == -1)
		strerr_die3sys(111, FATAL, fntmp.s, ": ");
	if (cdbmss_start(&cdbmss, fdtemp) == -1)
		strerr_die3sys(111, FATAL, fntmp.s, ": write: ");
	if (!stralloc_copys(&wildchars, ""))
		strerr_die2sys(111, FATAL, "out of memory");
	for (;;) {
		if (getln(&ssin, &line, &match, '\n') != 0)
			strerr_die4sys(111, FATAL, "read: ", argv[1], ": ");
		if (line.len && (line.s[0] == '.'))
			break;
		if (!match)
			strerr_die3x(100, FATAL, "bad format in ", argv[1]);
		if (byte_chr(line.s, line.len, '\0') < line.len)
			strerr_die3x(100, FATAL, "bad format in ", argv[1]);
		i = byte_chr(line.s, line.len, ':');
		if (i == line.len)
			strerr_die3x(100, FATAL, "bad format in ", argv[1]);
		if (i == 0)
			strerr_die3x(100, FATAL, "bad format in ", argv[1]);
		if (!stralloc_copys(&key, "!"))
			strerr_die2sys(111, FATAL, "out of memory");
		if (line.s[0] == '+') {
			if (!stralloc_catb(&key, line.s + 1, i - 1))
				strerr_die2sys(111, FATAL, "out of memory");
			case_lowerb(key.s, key.len);
			if (i >= 2) {
				if (byte_chr(wildchars.s, wildchars.len, line.s[i - 1]) == wildchars.len) {
					if (!stralloc_append(&wildchars, line.s + i - 1))
						strerr_die2sys(111, FATAL, "out of memory");
				}
			}
		} else {
			if (!stralloc_catb(&key, line.s + 1, i - 1) || !stralloc_0(&key))
				strerr_die2sys(111, FATAL, "out of memory");
			case_lowerb(key.s, key.len);
		}
		if (!stralloc_copyb(&data, line.s + i + 1, line.len - i - 1))
			strerr_die2sys(111, FATAL, "out of memory");
		numcolons = 0;
		for (i = 0; i < data.len; ++i) {
			if (data.s[i] == ':') {
				data.s[i] = 0;
				if (++numcolons == 1)
					break;
			}
		}
		if (numcolons < 1)
			strerr_die3x(100, FATAL, "bad format in ", argv[1]);
		data.len = i;
		if (cdbmss_add(&cdbmss, (unsigned char *) key.s, key.len, (unsigned char *) data.s, data.len) == -1)
			strerr_die3sys(111, FATAL, fntmp.s, ": write: ");
	}
	close(fd);
	if (cdbmss_add(&cdbmss, (unsigned char *) "", 0, (unsigned char *) wildchars.s, wildchars.len) == -1)
		strerr_die3sys(111, FATAL, fntmp.s, ": write: ");
	if (cdbmss_finish(&cdbmss) == -1)
		strerr_die3sys(111, FATAL, fntmp.s, ": write: ");
	if (fsync(fdtemp) == -1)
		strerr_die3sys(111, FATAL, fntmp.s, ": write: ");
	if (close(fdtemp) == -1)
		strerr_die3sys(111, FATAL, fntmp.s, ": write: ");
	if (rename(fntmp.s, fncdb.s) == -1)
		strerr_die6sys(111, FATAL, "rename: ", fntmp.s, " ==> ", fncdb.s, ": ");
	return(0);
}

void
getversion_cdb_database_c()
{
	static char    *x = "$Id: cdb-database.c,v 1.1 2021-06-15 11:30:39+05:30 Cprogrammer Exp mbhangui $";

	x++;
}
