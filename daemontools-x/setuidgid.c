/*
 * $Log: setuidgid.c,v $
 * Revision 1.5  2020-09-16 19:06:56+05:30  Cprogrammer
 * FreeBSD fix
 *
 * Revision 1.4  2020-06-16 23:57:33+05:30  Cprogrammer
 * added option -s to set supplementary groups
 *
 * Revision 1.3  2020-06-16 22:36:48+05:30  Cprogrammer
 * set supplementary group ids
 *
 * Revision 1.2  2004-10-22 20:30:17+05:30  Cprogrammer
 * added RCS id
 *
 * Revision 1.1  2003-12-31 18:37:58+05:30  Cprogrammer
 * Initial revision
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef FREEBSD
#include <sys/param.h>
#endif
#include <pwd.h>
#include <grp.h>
#include <setuserid.h>
#include <strerr.h>
#include <sgetopt.h>
#include "prot.h"
#include "pathexec.h"

#define FATAL "setuidgid: fatal: "
char           *usage = "usage: setuidgid [-s] account child";

struct passwd  *pw;

int
main(int argc, char **argv, char **envp)
{
	gid_t          *gidset;
	char           *account;
	char          **child;
	int             ngroups = 0, opt;

	while ((opt = getopt(argc, argv, "s")) != opteof) {
		switch (opt)
		{
		case 's':
			ngroups = 1;
			break;
		default:
			strerr_die1x(100, usage);
		}
	}
	if (argc < optind + 2)
		strerr_die1x(100, usage);
	account = argv[optind++];
	child = argv + optind++;
	if (!(pw = getpwnam(account)))
		strerr_die3x(111, FATAL, "unknown account ", account);
	if (prot_gid(pw->pw_gid) == -1)
		strerr_die2sys(111, FATAL, "unable to setgid: ");
	if (ngroups) {
		if (!(gidset = grpscan(account, &ngroups)))
			strerr_die2sys(111, FATAL, "unable to do groupscan: ");
		if (setgroups(ngroups, gidset))
			strerr_die2sys(111, FATAL, "unable to setgroups: ");
	}
	if (prot_uid(pw->pw_uid) == -1)
		strerr_die2sys(111, FATAL, "unable to setuid: ");

	pathexec_run(*child, child, envp);
	strerr_die4sys(111, FATAL, "unable to run ", *child, ": ");
	/*- Not reached */
	return(1);
}

void
getversion_setuidgid_c()
{
	static char    *x = "$Id: setuidgid.c,v 1.5 2020-09-16 19:06:56+05:30 Cprogrammer Exp mbhangui $";

	x++;
}
