/*
 *  $Log: tcpserver_plugin.c,v $
 *  Revision 1.4  2017-04-05 03:13:55+05:30  Cprogrammer
 *  changed dlopen() to dlmopen() for private namespace
 *
 *  Revision 1.3  2016-05-23 04:44:17+05:30  Cprogrammer
 *  added two arguments to strerr_die()
 *
 *  Revision 1.2  2016-05-16 21:17:01+05:30  Cprogrammer
 *  added reload_flag for use in sighup handler
 *
 *  Revision 1.1  2016-05-15 22:37:54+05:30  Cprogrammer
 *  Initial revision
 *
 */

#ifndef	lint
static char     sccsid[] = "$Id: tcpserver_plugin.c,v 1.4 2017-04-05 03:13:55+05:30 Cprogrammer Exp mbhangui $";
#endif

#define FATAL "tcpserver: fatal: "

#ifdef LOAD_SHARED_OBJECTS
#define _GNU_SOURCE
#include <unistd.h>
#include "stralloc.h"
#include "dlnamespace.h"
#include "strerr.h"
#include "str.h"
#include "env.h"
#include "scan.h"
#include "fmt.h"
#include <link.h>
#include <dlfcn.h>
struct stralloc shared_objfn = {0};

int
tcpserver_plugin(char **envp, int reload_flag)
{
	char            env_str[FMT_ULONG + 12];
	void           *handle;
	unsigned int    plugin_no;
	int             (*func) (int);
	int             i;
	char           *error, *c, *func_name, *s;
	char          **ptr1;
	char            strnum[FMT_ULONG];
	Lmid_t          lmid;

	for (ptr1 = envp; *ptr1; ptr1++) {
		if (!str_start(*ptr1, "PLUGIN"))
			continue;
		c = *ptr1 + 6;
		if (!(i = scan_uint(c, &plugin_no))) /*- no number following PLUGIN */
			continue;
		if (*(c + i) != '=') /*- anthing other than PLUGIN<num> */
			continue;
		for (c = *ptr1;*c && *c != '=';c++);
		if (!stralloc_copys(&shared_objfn, c + 1))
			strerr_die2x(111, FATAL, "out of memory");
		if (!stralloc_0(&shared_objfn))
			strerr_die2x(111, FATAL, "out of memory");
		if (reload_flag) {
#ifdef RTLD_DEEPBIND
			if (!(handle = dlmopen(LM_ID_NEWLM, shared_objfn.s, RTLD_NOW|RTLD_LOCAL|RTLD_DEEPBIND|RTLD_NODELETE))) {
#else
			if (!(handle = dlmopen(LM_ID_NEWLM, shared_objfn.s, RTLD_NOW|RTLD_LOCAL|RTLD_NODELETE))) {
#endif
				strerr_die(111, FATAL, "dlmopen: ", shared_objfn.s, ": ", dlerror(), 0, 0, 0, (struct strerr *) 0);
				return (1);
			} else
			if (dlinfo(handle, RTLD_DI_LMID, &lmid) == -1)
				strerr_die(111, FATAL, "dlinfo: ", shared_objfn.s, ": ", dlerror(), 0, 0, 0, (struct strerr *) 0);
			if (dlnamespace(shared_objfn.s, (unsigned long *) &lmid) < 0)
				strerr_die(111, FATAL, "dlnamespace: ", shared_objfn.s, ": unable to store namespace", 0, 0, 0, 0, (struct strerr *) 0);
		} else {
			lmid = 0;
			if ((i = dlnamespace(shared_objfn.s, (unsigned long *) &lmid)) < 0)
				strerr_die(111, FATAL, "dlnamespace: ", shared_objfn.s, ": ", 0, 0, 0, 0, (struct strerr *) 0);
			else
			if (!i)
				strerr_die(111, FATAL, "dlnamespace: ", shared_objfn.s, ": unable to get namespace", 0, 0, 0, 0, (struct strerr *) 0);
			if (!(handle = dlmopen(lmid, shared_objfn.s, RTLD_NOW|RTLD_NOLOAD))) {
				strerr_die(111, FATAL, "dlmopen: ", shared_objfn.s, ": ", dlerror(), 0, 0, 0, (struct strerr *) 0);
				return (1);
			} else
			if (dlinfo(handle, RTLD_DI_LMID, &lmid) == -1)
				strerr_die(111, FATAL, "dlinfo: ", shared_objfn.s, ": ", dlerror(), 0, 0, 0, (struct strerr *) 0);
		}
		/*- execute function defined by PLUGIN<num>_init */
		s = env_str;
		s += (i = fmt_str((char *) s, "PLUGIN"));
		s += (i = fmt_uint((char *) s, plugin_no));
		s += (i = fmt_str((char *) s, "_init"));
		*s++ = 0;
		dlerror(); /*- clear existing error */
		if (reload_flag) {
			strnum[fmt_ulong(strnum, lmid)] = 0;
			i = str_chr(shared_objfn.s, '.');
			if (i)
				shared_objfn.s[i--] = 0;
			for (c = shared_objfn.s + i;*c && *c != '/';c--);
			if (*c == '/')
				c++;
			strerr_warn4("tcpserver: ", c, ".so: link map ID: ", strnum, 0);
		}
		if ((func_name = env_get(env_str))) {
			func = dlsym(handle, func_name);
			if ((error = dlerror()))
				strerr_die(111, FATAL, "dlsym: ", func_name, ": ", error, 0, 0, 0, (struct strerr *) 0);
			/*- change to dir defined by PLUGIN<num>_dir */
			s = env_str;
			s += (i = fmt_str((char *) s, "PLUGIN"));
			s += (i = fmt_uint((char *) s, plugin_no));
			s += (i = fmt_str((char *) s, "_dir"));
			*s++ = 0;
			if ((s = env_get(env_str)) && chdir(s))
				strerr_die(111, FATAL, "chdir: ", s, ": ", 0, 0, 0, 0, (struct strerr *) 0);
			(*func) (!reload_flag); /*- execute the function */
		}
	} /*- for (ptr1 = envp; *ptr1; ptr1++) { */
	return (0);
}
#else
int
tcpserver_plugin(char **envp)
{
	return (0);
}
#endif

void
getversion_tcpserver_plugin_c()
{
	write(1, sccsid, 0);
}
