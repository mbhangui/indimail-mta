/*
 * $Log: set_environment.c,v $
 * Revision 1.1  2021-05-13 12:37:36+05:30  Cprogrammer
 * Initial revision
 *
 */
#include <unistd.h>
#include <env.h>
#include <envdir.h>
#include <strerr.h>
#include <pathexec.h>
#include "auto_sysconfdir.h"
#include "auto_control.h"
#include "variables.h"

void
set_environment(char *warn, char *fatal)
{
	char           *qbase, *home, *err;
	char           **e;
	int             i;

	if ((home = env_get("HOME"))) {
		if (chdir(home) == -1)
			strerr_die4sys(111, fatal, "unable to switch to ", home, ": ");
		if (!access(".defaultqueue", X_OK)) {
			if ((i = envdir(".defaultqueue", 0)))
				strerr_warn3(warn, envdir_str(i), ":", &strerr_sys);
			if ((e = pathexec(0)))
				environ = e;
		}
	}
	if (chdir(auto_sysconfdir) == -1)
		strerr_die4sys(111, fatal, "unable to switch to ", auto_sysconfdir, ": ");
	if (!(qbase = env_get("QUEUE_BASE"))) {
		if (!controldir) {
			if (!(controldir = env_get("CONTROLDIR")))
				controldir = auto_control;
		}
		if (chdir(controldir) == -1)
			strerr_die4sys(111, fatal, "unable to switch to ", controldir, ": ");
		if (!access("defaultqueue", X_OK)) {
			switch(envdir("defaultqueue", &err))
			{
			case -1:
				strerr_die4sys(111, fatal, "unable to read environment file defaultqueue/", err, ": ");
			case -2:
				strerr_die2sys(111, fatal, "unable to open current directory: ");
			case -3:
				strerr_die2sys(111, fatal, "unable to switch to environment directory defaultqueue: ");
			case -4:
				strerr_die2sys(111, fatal, "unable to read environment directory defaultqueue: ");
			case -5:
				strerr_die2sys(111, fatal, "unable to switch back to original directory: ");
			case -6:
				strerr_die2x(111, fatal, "out of memory");
			}
			if ((e = pathexec(0)))
				environ = e;
		}
		if (chdir(auto_sysconfdir) == -1)
			strerr_die4sys(111, fatal, "unable to switch to ", auto_sysconfdir, ": ");
	}
	return;
}

void
getversion_set_environment_c()
{
	static char    *x = "$Id: set_environment.c,v 1.1 2021-05-13 12:37:36+05:30 Cprogrammer Exp mbhangui $";

	x++;
}