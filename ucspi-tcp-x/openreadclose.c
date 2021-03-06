/*
 * $Log: openreadclose.c,v $
 * Revision 1.2  2020-08-03 17:25:16+05:30  Cprogrammer
 * use qmail library
 *
 * Revision 1.1  2003-12-31 19:46:55+05:30  Cprogrammer
 * Initial revision
 *
 */
#include <error.h>
#include <open.h>
#include "readclose.h"
#include "openreadclose.h"

int
openreadclose(char *fn, stralloc * sa, unsigned int bufsize)
{
	int             fd;

	if ((fd = open_read(fn)) == -1) {
		if (errno == error_noent)
			return 0;
		return -1;
	}
	if (readclose(fd, sa, bufsize) == -1)
		return -1;
	return 1;
}
