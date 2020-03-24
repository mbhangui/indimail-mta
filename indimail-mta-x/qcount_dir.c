/*
 * $Log: qcount_dir.c,v $
 * Revision 1.1  2020-03-24 12:57:30+05:30  Cprogrammer
 * Initial revision
 *
 */
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "qcount_dir.h"
#include "fmt.h"
#include "scan.h"
#include "env.h"
#include "str.h"
#include "alloc.h"
#include "strerr.h"

static int
skip_system_files(char *filename)
{
	char           *system_files[] = {
		".Trash",
		".current_size",
		"domain",
		"QuotaWarn",
		"vfilter",
		"folder.dateformat",
		"noprefilt",
		"nopostfilt",
		"BulkMail",
		"deliveryCount", 
		"maildirfolder",
		"maildirsize",
		"core",
		"sqwebmail",
		"courier",
		"shared-maildirs",
		"shared-timestamp",
		"shared-folders",
		0,
	};
	char          **ptr;
	int             len;

	for (ptr = system_files; ptr && *ptr; ptr++) {
		len = str_len(*ptr);
		if (!str_diffn(filename, *ptr, len + 1))
			return (1);
	}
	return (0);
}

ssize_t qcount_dir(char *dir_name, size_t *mailcount)
{
	DIR            *entry;
	struct dirent  *dp;
	struct stat     statbuf;
	size_t          file_size, tmp, oldlen, newlen, dirname_len, filename_len, count;
	int             is_trash, i;
	char           *tmpstr, *ptr, *include_trash;
	char            strnum[FMT_ULONG];

	if (!dir_name || !*dir_name)
		return (0);
	if (!(entry = opendir(dir_name)))
		return (0);
	if ((include_trash = (char *) env_get("INCLUDE_TRASH")))
		is_trash = 0;
	else {
		if (str_str(dir_name, "/Maildir/.Trash"))
			is_trash = 1;
		else
			is_trash = 0;
	}
	if (mailcount)
		*mailcount = 0;
	dirname_len = str_len(dir_name);
	for (tmpstr = 0, file_size = oldlen = newlen = 0;;) {
		if (!(dp = readdir(entry)))
			break;
		if (!str_diffn(dp->d_name, ".", 2) || !str_diffn(dp->d_name, "..", 3))
			continue;
		else
		if (skip_system_files(dp->d_name))
			continue;
		filename_len = str_len(dp->d_name);
		newlen = sizeof(char) * (filename_len + dirname_len + 2);
		if (!alloc_re((char *) &tmpstr, oldlen, newlen)) {
			strnum[i = fmt_uint(strnum, newlen)] = 0;
			strerr_warn3("qcount_dir: alloc_re: ", strnum, " bytes: ", &strerr_sys);
			closedir(entry);
			return (-1);
		}
		oldlen = newlen;
		ptr = tmpstr;
		ptr += fmt_strn(ptr, dir_name, dirname_len);
		ptr += fmt_strn(ptr, "/", 1);
		ptr += fmt_strn(ptr, dp->d_name, filename_len);
		*ptr++ = 0;
		if ((ptr = str_str(tmpstr, ",S=")) != NULL) {
			ptr += 3;
			scan_ulong(ptr, (unsigned long *) &tmp);
			file_size += tmp;
			if (mailcount)
				(*mailcount)++;
		} else
		if (!stat(tmpstr, &statbuf)) {
			if (S_ISDIR(statbuf.st_mode)) {
				file_size += qcount_dir(tmpstr, &count);
				if (mailcount)
					*mailcount += count;
			} else {
				if (!include_trash && (*(dp->d_name + filename_len - 1) == 'T' || is_trash))
					continue;
				if (mailcount)
					(*mailcount)++;
				file_size += statbuf.st_size;
			}
		}
	}
	closedir(entry);
	alloc_free(tmpstr);
	return (file_size);
}

void
getversion_qcount_dir_c()
{
	static char    *x = "$Id: qcount_dir.c,v 1.1 2020-03-24 12:57:30+05:30 Cprogrammer Exp mbhangui $";

	x++;
}