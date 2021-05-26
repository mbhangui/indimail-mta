/*
 * $Log: getDomainToken.c,v $
 * Revision 1.1  2021-05-26 05:20:05+05:30  Cprogrammer
 * Initial revision
 *
 */
#include <str.h>
#include <regex.h>
#include <stralloc.h>
#include <env.h>
#include <regex.h>
#include "wildmat.h"
#include "report.h"

#define REGCOMP(X,Y)    regcomp(&X, Y, REG_EXTENDED|REG_ICASE)
#define REGEXEC(X,Y)    regexec(&X, Y, (size_t) 0, (regmatch_t *) 0, (int) 0)
#ifndef REG_NOERROR
#define REG_NOERROR 0
#endif

extern dtype    delivery;

char           *
getDomainToken(char *domain, stralloc *sa)
{
	regex_t         qreg;
	int             len, n, retval;
	char           *ptr, *p;

	for (len = 0, ptr = sa->s;len < sa->len;) {
		len += ((n = str_len(ptr)) + 1);
		for (p = ptr;*p && *p != ':';p++);
		if (*p == ':') {
			*p = 0;
			/*- build the regex */
			if ((retval = str_diff(ptr, domain))) {
				if (env_get("QREGEX")) {
					if ((retval = REGCOMP(qreg, ptr)) == 0)
						retval = (REGEXEC(qreg, domain) == REG_NOMATCH ? 1 : REG_NOERROR);
					regfree(&qreg);
				} else
					retval = !wildmat_internal(domain, ptr);
			}
			*p = ':';
			if (!retval) { /*- match occurred for domain or wildcard */
				/* check for local/remote directives */
				if (delivery == remote) { /*- remote delivery */
					if (!str_diffn(p + 1, "remote:", 7))
						return (p + 8);
					if (!str_diffn(p + 1, "local:", 6)) {
						ptr = sa->s + len;
						continue; /*- skip local directives for remote mails */
					}
				} else { /*- local delivery */
					if (!str_diffn(p + 1, "local:", 6))
						return (p + 7);
					if (!str_diffn(p + 1, "remote:", 7)) {
						ptr = sa->s + len;
						continue; /*- skip remote directives for local mails */
					}
				}
				return (p + 1);
			}
		}
		ptr = sa->s + len;
	} /*- for (len = 0, ptr = sa->s;len < sa->len;) */
	return ((char *) 0);
}

void
getversion_getdomaintoke_c()
{
	static char    *x = "$Id: getDomainToken.c,v 1.1 2021-05-26 05:20:05+05:30 Cprogrammer Exp mbhangui $";

	x = sccsidwildmath;
	x = sccsidreporth;
	x++;
}
