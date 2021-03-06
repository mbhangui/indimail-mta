/*
 * $Log: maildirserial.c,v $
 * Revision 1.17  2021-07-05 21:26:29+05:30  Cprogrammer
 * allow processing $HOME/.defaultqueue for root
 *
 * Revision 1.16  2021-06-14 00:49:50+05:30  Cprogrammer
 * added missing chdir removed by mistake
 *
 * Revision 1.15  2021-06-03 18:14:12+05:30  Cprogrammer
 * use new prioq functions
 *
 * Revision 1.14  2021-05-13 14:43:28+05:30  Cprogrammer
 * use set_environment() to set env from ~/.defaultqueue or control/defaultqueue
 *
 * Revision 1.13  2020-05-11 11:02:50+05:30  Cprogrammer
 * fixed shadowing of global variables by local variables
 *
 * Revision 1.12  2020-04-04 11:48:46+05:30  Cprogrammer
 * use auto_sysconfdir instead of auto_qmail
 *
 * Revision 1.11  2020-04-04 11:23:28+05:30  Cprogrammer
 * use environment variables $HOME/.defaultqueue before /etc/indimail/control/defaultqueue
 *
 * Revision 1.10  2016-05-21 14:48:06+05:30  Cprogrammer
 * use auto_sysconfdir for leapsecs_init()
 *
 * Revision 1.9  2016-05-17 19:44:58+05:30  Cprogrammer
 * use auto_control, set by conf-control to set control directory
 *
 * Revision 1.8  2010-07-21 08:57:49+05:30  Cprogrammer
 * use CONTROLDIR environment variable instead of a hardcoded control directory
 *
 * Revision 1.7  2010-06-08 21:59:30+05:30  Cprogrammer
 * use envdir_set() on queuedefault to set default queue parameters
 *
 * Revision 1.6  2004-10-22 20:26:29+05:30  Cprogrammer
 * added RCS id
 *
 * Revision 1.5  2004-10-22 15:35:41+05:30  Cprogrammer
 * removed readwrite.h
 *
 * Revision 1.4  2004-10-11 23:50:51+05:30  Cprogrammer
 * made MIME configurable at compile time
 * use config_data() for geting bouncehost
 *
 * Revision 1.3  2004-08-04 18:24:00+05:30  Cprogrammer
 * enclose bounce as MIME
 *
 * Revision 1.2  2004-07-17 21:19:31+05:30  Cprogrammer
 * code beatification
 * added RCS log
 *
 * Revision 1.1  2004-05-14 00:44:57+05:30  Cprogrammer
 * Initial revision
 *
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "env.h"
#include "envdir.h"
#include "pathexec.h"
#include "sgetopt.h"
#include "scan.h"
#include "stralloc.h"
#include "fd.h"
#include "open.h"
#include "getln.h"
#include "subfd.h"
#include "strerr.h"
#include "substdio.h"
#include "maildir.h"
#include "prioq.h"
#include "wait.h"
#include "sig.h"
#include "str.h"
#include "fmt.h"
#include "tai.h"
#include "mess822.h"
#include "now.h"
#include "sconfig.h"
#include "qmail.h"
#include "quote.h"
#include "byte.h"
#include "auto_sysconfdir.h"
#include "auto_control.h"
#include "variables.h"
#include "set_environment.h"

#define FATAL "maildirserial: fatal: "
#define WARNING "maildirserial: warning: "
#define INFO "maildirserial: info: "

void
die_usage()
{
	strerr_die1x(100, "maildirserial: usage: maildirserial [ -b ] [ -t lifetime ] dir prefix client [ arg ... ]");
}

void
die_readclient()
{
	strerr_die2sys(111, FATAL, "unable to read from client: ");
}

void
die_nomem()
{
	strerr_die2x(111, FATAL, "out of memory");
}

void
die_qq()
{
	strerr_die2sys(111, FATAL, "unable to run qq: ");
}

char           *prefix;
char          **client;
char            messbuf[256];
substdio        ssmess;
stralloc        line = { 0 };
stralloc        recipient = { 0 };
stralloc        fn = {0};
char            buf[1024];
substdio        ss;				/*- in parent, reading from child; in scanner, writing to child */
int             pid;			/*- in parent, pid of scanner; in scanner, pid of child */
int             wstat;
stralloc        deadfiles = { 0 };


/*
 * ---------------------------------------------------------------- BOUNCING 
 */

config_str      me = CONFIG_STR;
config_str      bouncefrom = CONFIG_STR;
config_str      bouncehost = CONFIG_STR;
config_str      doublebounceto = CONFIG_STR;
config_str      doublebouncehost = CONFIG_STR;
#ifdef MIME
stralloc        boundary = { 0 };
#endif

static void
my_config_readline(config_str *c, char *fname)
{
	if (!controldir) {
		if (!(controldir = env_get("CONTROLDIR")))
			controldir = auto_control;
	}
	if (!stralloc_copys(&fn, controldir) ||
			!stralloc_cats(&fn, "/") ||
			!stralloc_cats(&fn, fname) ||
			!stralloc_0(&fn))
		die_nomem();
	if (config_readline(c, fn.s) == -1)
		strerr_die4sys(111, FATAL, "unable to read ", fn.s, ": ");
}

void
readcontrols()
{
	set_environment(WARNING, FATAL, 1);
	my_config_readline(&me, "me");
	if (config_default(&me, "me") == -1)
		die_nomem();
	my_config_readline(&bouncefrom, "bouncefrom");
	if (config_default(&bouncefrom, "MAILER-DAEMON") == -1)
		die_nomem();
	my_config_readline(&bouncehost, "bouncehost");
	if (config_copy(&bouncehost, &me) == -1)
		die_nomem();
	my_config_readline(&doublebouncehost, "doublebouncehost");
	if (config_copy(&doublebouncehost, &me) == -1)
		die_nomem();
	my_config_readline(&doublebounceto, "doublebounceto");
	if (config_default(&doublebounceto, "postmaster") == -1)
		die_nomem();
	if (!stralloc_cats(config_data(&doublebounceto), "@") ||
			!stralloc_cat(config_data(&doublebounceto), config_data(&doublebouncehost)) ||
			!stralloc_0(config_data(&doublebounceto)))
		die_nomem();
}

struct qmail    qq;

void
put(char *buffer, int len)
{
	qmail_put(&qq, buffer, len);
}

void
my_puts(char *buffer)
{
	qmail_puts(&qq, buffer);
}

char           *qqx;
unsigned long   qp;
char            num[FMT_ULONG];

struct tai      datetai;
mess822_time    date;
stralloc        datestr = { 0 };

stralloc        sender = { 0 };
stralloc        quoted = { 0 };

int
bounce(int fd, stralloc *why, int flagtimeout) /*- why must end with \n; must not contain \n\n */
{
	int             match, n;
	char           *bouncesender, *bouncerecip, *x;

	substdio_fdbuf(&ssmess, read, fd, messbuf, sizeof messbuf);

	if (getln(&ssmess, &line, &match, '\n') == -1)
		return -1;
	if (!match)
		return -3;
	if (!stralloc_starts(&line, "Return-Path: <"))
		return -3;
	if (line.s[line.len - 2] != '>')
		return -3;
	if (line.s[line.len - 1] != '\n')
		return -3;
	if (!stralloc_copyb(&sender, line.s + 14, line.len - 16))
		die_nomem();
	if (byte_chr(sender.s, sender.len, '\0') < sender.len)
		return -3;
	if (!stralloc_0(&sender))
		die_nomem();
	if (getln(&ssmess, &line, &match, '\n') == -1)
		return -1;
	if (!match)
		return -3;
	if (!stralloc_starts(&line, "Delivered-To: "))
		return -3;
	if (line.s[line.len - 1] != '\n')
		return -3;
	if (!stralloc_copyb(&recipient, line.s + 14, line.len - 15))
		die_nomem();
	if (str_equal(sender.s, "#@[]"))
		return 2;
	if (qmail_open(&qq) == -1)
		die_qq();
	qp = qmail_qp(&qq);

	if (*sender.s) {
		bouncesender = "";
		bouncerecip = sender.s;
	} else {
		bouncesender = "#@[]";
		bouncerecip = config_data(&doublebounceto)->s;
	}
	tai_now(&datetai);
	caltime_utc(&date.ct, &datetai, (int *) 0, (int *) 0);
	date.known = 1;
	if (!mess822_date(&datestr, &date))
		die_nomem();
	my_puts("Date: ");
	put(datestr.s, datestr.len);
	my_puts("\n");
	my_puts("From: ");
	if (!quote(&quoted, config_data(&bouncefrom)))
		die_nomem();
	put(quoted.s, quoted.len);
	my_puts("@");
	put(config_data(&bouncehost)->s, config_data(&bouncehost)->len);
	my_puts("\nTo: ");
	if (!quote2(&quoted, bouncerecip))
		die_nomem();
	put(quoted.s, quoted.len);
#ifdef MIME
	/* MIME header with boundary */
	my_puts("\nMIME-Version: 1.0\n"
			"Content-Type: multipart/mixed; "
			"boundary=\"");
	if (!stralloc_copyb(&boundary, num, fmt_ulong(num, datetai.x)) ||
			!stralloc_cats(&boundary, ".qp_") ||
			!stralloc_catb(&boundary, num, fmt_ulong(num, qp)) ||
			!stralloc_cats(&boundary, ".KUI@") ||
			!stralloc_cats(&boundary, config_data(&bouncehost)->s))
		die_nomem();
	put(boundary.s, boundary.len);
	my_puts("\"");
	my_puts("\nSubject: failure notice\n\n--");
	put(boundary.s, boundary.len);	/* def type is text/plain */
#else
	my_puts("\nSubject: failure notice");
#endif
	my_puts("\n\n");
	my_puts("Hi. This is the maildirbounce program at ");
	put(config_data(&bouncehost)->s, config_data(&bouncehost)->len);
	my_puts(".\n");
	my_puts(*sender.s ?
		"I'm afraid I wasn't able to deliver your message to the following address.\n"
		"This is a permanent error; I've given up. Sorry it didn't work out.\n\n"
		:
		"I tried to deliver a bounce message to this address, but the bounce bounced!\n\n"
	);
	my_puts("<");
	if (stralloc_starts(&recipient, prefix))
		put(recipient.s + str_len(prefix), recipient.len - str_len(prefix));
	else
		put(recipient.s, recipient.len);
	my_puts(">:\n");
	put(why->s, why->len);
	if (flagtimeout)
		my_puts("This message is too old. Giving up.\n");
	my_puts("\n");
#ifdef MIME
	my_puts(*sender.s ? "--- Enclosed is a copy of the message.\n\n--" : "--- Enclosed is the original bounce.\n\n--");
	put(boundary.s,boundary.len);	/* enclosure boundary */
	my_puts("\nContent-Type: message/rfc822\n\n");
#else
	my_puts(*sender.s ? "--- Below this line is a copy of the message.\n\n" : "--- Below this line is the original bounce.\n\n");
#endif
	my_puts("Return-Path: <");
	if (!quote2(&quoted, sender.s))
		die_nomem();
	put(quoted.s, quoted.len);
	my_puts(">\n");
	for (;;) {
		n = substdio_feed(&ssmess);
		if (n < 0)
			strerr_die2sys(111, FATAL, "unable to read message: ");
		if (!n)
			break;
		x = substdio_PEEK(&ssmess);
		put(x, n);
		substdio_SEEK(&ssmess, n);
	}
#ifdef MIME
	my_puts("\n--"); /* end boundary */
	put(boundary.s,boundary.len);
	my_puts("--\n");
#endif
	qmail_from(&qq, bouncesender);
	qmail_to(&qq, bouncerecip);
	qqx = qmail_close(&qq);
	if (*qqx)
		return -2;
	return 1;
}


/*
 * ----------------------------------------------------------------- SCANNER 
 */

int
hasprefix(int fd)
{
	int             match;

	substdio_fdbuf(&ssmess, read, fd, messbuf, sizeof messbuf);
	if (getln(&ssmess, &line, &match, '\n') == -1)
		return -1;
	if (!match)
		return 0;
	if (getln(&ssmess, &line, &match, '\n') == -1)
		return -1;
	if (!match)
		return 0;
	if (!stralloc_starts(&line, "Delivered-To: "))
		return 0;
	if (line.s[line.len - 1] != '\n')
		return 0;
	if (!stralloc_copyb(&recipient, line.s + 14, line.len - 15))
		return -1;
	return stralloc_starts(&recipient, prefix);
}

int
usable(char *filename)
{
	int             i;
	int             fd;

	i = 0;
	while (i < deadfiles.len) {
		if (str_equal(filename, deadfiles.s + i))
			return 0;
		i += str_len(deadfiles.s + i);
		++i;
	}

	if (!*prefix)
		return 1;	/*- optimization */
	if ((fd = open_read(filename)) == -1) {
		strerr_warn4(WARNING, "unable to open ", filename, ": ", &strerr_sys);
		return 0;
	}
	if ((i = hasprefix(fd)) == -1)
		strerr_warn4(WARNING, "unable to read ", filename, ": ", &strerr_sys);
	close(fd);
	return i == 1;
}

stralloc        filenames = { 0 };
prioq           pq = { 0 };

int             pis2c[2];

void
scanner()
{
	struct prioq_elt pe;
	char           *filename;

	if (pipe(pis2c) == -1)
		strerr_die2sys(111, FATAL, "unable to create pipe: ");
	substdio_fdbuf(&ss, write, pis2c[1], buf, sizeof buf);
	maildir_clean(&filenames);
	if (maildir_scan(&pq, &filenames, 1, 1) == -1)
		strerr_die1(111, FATAL, &maildir_scan_err);
	while (prioq_get(&pq, &pe)) {
		prioq_del(&pq);
		filename = filenames.s + pe.id;
		if (usable(filename)) {
			if (!pid) {
				pid = fork();
				if (pid == -1)
					strerr_die2sys(111, FATAL, "unable to fork: ");
				if (pid == 0) {
					close(pis2c[1]);
					fd_move(0, pis2c[0]);
					sig_pipedefault();
					execvp(*client, client);
					strerr_die4sys(111, FATAL, "unable to run ", *client, ": ");
				}
				close(pis2c[0]);
			}
			substdio_put(&ss, filename, str_len(filename) + 1);	/*- ignore errors */
		}
	}
	if (!pid)
		_exit(0);
	substdio_flush(&ss);		/*- ignore errors */
	close(pis2c[1]);
	if (wait_pid(&wstat, pid) == -1)
		strerr_die2sys(111, FATAL, "unable to get client status: ");
	if (wait_crashed(wstat))
		strerr_die2x(111, FATAL, "client crashed");
	if (wait_exitcode(wstat) == 100)
		_exit(100);				/*- client has produced error message */
	_exit(30);
}


/*
 * ------------------------------------------------------------------ PARENT 
 */

int             flagbounce = 0;
int             flaglifetime = 0;
unsigned long   lifetime;
int             flagtimeout;

int             pic2p[2];

stralloc        err = { 0 };
int             match;

void
info(char *result)
{
	substdio_puts(subfderr, INFO);
	substdio_puts(subfderr, fn.s);
	substdio_puts(subfderr, result);
	substdio_put(subfderr, err.s, err.len);
	substdio_flush(subfderr);
}

int
main(int argc, char **argv)
{
	int             opt, r, progress;
	char           *dir;
	char            status;
	struct stat     st;

	sig_pipeignore();

	while ((opt = getopt(argc, argv, "bt:")) != opteof) {
		switch (opt)
		{
		case 'b':
			flagbounce = 1;
			break;
		case 't':
			scan_ulong(optarg, &lifetime);
			flaglifetime = 1;
			break;
		default:
			die_usage();
		}
	}
	argv += optind;
	dir = *argv++;
	if (!dir)
		die_usage();
	prefix = *argv++;
	if (!prefix)
		die_usage();
	client = argv;
	if (!*client)
		die_usage();
	readcontrols();

	if (chdir(dir) == -1)
		strerr_die4sys(111, FATAL, "unable to chdir to ", dir, ": ");
	if (!stralloc_copys(&deadfiles, ""))
		die_nomem();
	progress = 3;
	for (;;) {
		if (pipe(pic2p) == -1)
			strerr_die2sys(111, FATAL, "unable to create pipe: ");
		if ((pid = fork()) == -1)
			strerr_die2sys(111, FATAL, "unable to fork: ");
		if (!pid) {
			close(pic2p[0]);
			fd_move(1, pic2p[1]);
			scanner();
		}
		close(pic2p[1]);
		substdio_fdbuf(&ss, read, pic2p[0], buf, sizeof buf);
		--progress;
		for (;;) {
			if (getln(&ss, &fn, &match, '\0') == -1)
				die_readclient();
			if (!match)
				break;
			if ((r = substdio_get(&ss, &status, 1)) == -1)
				die_readclient();
			if (!match)
				break;
			if (getln(&ss, &err, &match, '\n') == -1)
				die_readclient();
			if (!match)
				break;
			progress = 3;
			if (status == 'K') {
				info(" succeeded: ");
				if (unlink(fn.s) == -1) {
					strerr_warn4(WARNING, "message will be delivered twice! unable to unlink ", fn.s, ": ", &strerr_sys);
					if (!stralloc_cat(&deadfiles, &fn))
						die_nomem();
				}
				continue;
			}
			flagtimeout = 0;
			if (flaglifetime && (status != 'D')) {
				if (stat(fn.s, &st) == -1)
					strerr_warn4(WARNING, "assuming message is still alive; unable to stat ", fn.s, ": ", &strerr_sys);
				else
				if (now() > st.st_mtime + lifetime) {
					status = 'D';
					flagtimeout = 1;
				}
			}
			if (flagbounce && status == 'D') {
				int             fd, t;

				info(" bounced: ");
				if ((fd = open_read(fn.s)) == -1) {
					strerr_warn4(WARNING, "unable to read ", fn.s, ": ", &strerr_sys);
					if (!stralloc_cat(&deadfiles, &fn))
						die_nomem();
					continue;
				}
				if ((t = bounce(fd, &err, flagtimeout)) == -1)
					strerr_warn4(WARNING, "unable to bounce ", fn.s, ": ", &strerr_sys);
				if (t == -2)
					strerr_warn5(WARNING, "unable to bounce ", fn.s, ": qq failed: ", qqx + 1, 0);
				if (t == -3)
					strerr_warn4(WARNING, "unable to bounce ", fn.s, ": bad file format", 0);
				if (t == 2)
					strerr_warn4(INFO, "discarding ", fn.s, ": triple bounce", 0);
				if (t == 1) {
					substdio_puts(subfderr, INFO);
					substdio_puts(subfderr, "returned ");
					substdio_puts(subfderr, fn.s);
					substdio_puts(subfderr, ": qp ");
					substdio_put(subfderr, num, fmt_ulong(num, qp));
					substdio_puts(subfderr, "\n");
					substdio_flush(subfderr);
				}
				close(fd);
				if (t > 0) {
					if (unlink(fn.s) == 0)
						continue;
					strerr_warn4(WARNING, "message has been bounced but not removed! unable to unlink ", fn.s, ": ",
								 &strerr_sys);
				}
				if (!stralloc_cat(&deadfiles, &fn))
					die_nomem();
				continue;
			} /*- if (flagbounce && status == 'D') */
			info(status == 'D' ? " failed: " : " failed temporarily: ");
			if (!stralloc_cat(&deadfiles, &fn))
				die_nomem();
		}
		close(pic2p[0]);
		if (wait_pid(&wstat, pid) == -1)
			strerr_die2sys(111, FATAL, "unable to get scanner status: ");
		if (wait_crashed(wstat))
			strerr_die2x(111, FATAL, "scanner crashed");
		switch (wait_exitcode(wstat))
		{
		case 0:
			_exit(deadfiles.len ? 111 : 0);	/*- scanner says no files */
		case 100:
			_exit(100);			/*- scanner has produced error message */
		case 111:
			_exit(111);			/*- scanner has produced error message */
		}
		if (!progress)
			strerr_die2x(111, FATAL, "making no progress, giving up");
	}
	return(0);
}

void
getversion_maildirserial_c()
{
	static char    *x = "$Id: maildirserial.c,v 1.17 2021-07-05 21:26:29+05:30 Cprogrammer Exp mbhangui $";

	x++;
}
