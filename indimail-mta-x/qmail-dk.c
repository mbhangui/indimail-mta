/*
 * $Log: qmail-dk.c,v $
 * Revision 1.54  2021-06-15 22:14:47+05:30  Cprogrammer
 * pass tmpdir argument to pidopen
 *
 * Revision 1.53  2021-06-15 11:52:41+05:30  Cprogrammer
 * moved pidopen out to its own file
 *
 * Revision 1.52  2021-06-09 21:14:19+05:30  Cprogrammer
 * use qmulti() instead of exec of qmail-multi
 *
 * Revision 1.51  2021-05-26 10:44:10+05:30  Cprogrammer
 * handle access() error other than ENOENT
 *
 * Revision 1.50  2020-05-11 11:04:23+05:30  Cprogrammer
 * fixed shadowing of global variables by local variables
 *
 * Revision 1.49  2020-04-01 16:14:34+05:30  Cprogrammer
 * added header for makeargs() function
 *
 * Revision 1.48  2016-06-13 14:14:27+05:30  Cprogrammer
 * BUG - removed extra semicolon after if () statement
 *
 * Revision 1.47  2016-06-03 09:57:53+05:30  Cprogrammer
 * moved qmail-multi to sbin
 *
 * Revision 1.46  2016-05-17 19:44:58+05:30  Cprogrammer
 * use auto_control, set by conf-control to set control directory
 *
 * Revision 1.45  2015-12-24 14:36:56+05:30  Cprogrammer
 * fixed qmail-dk quitting during verfication without writing DomainKey-Status header
 *
 * Revision 1.44  2014-01-22 22:44:04+05:30  Cprogrammer
 * treat AUTHINFO environment like RELAYCLIENT environment variable
 *
 * Revision 1.43  2013-10-01 17:12:07+05:30  Cprogrammer
 * fixed QMAILQUEUE recursion
 *
 * Revision 1.42  2013-09-16 22:16:10+05:30  Cprogrammer
 * corrected logic for RELAYCLIENT_NODKVERIFY
 *
 * Revision 1.41  2013-09-13 16:33:59+05:30  Cprogrammer
 * turn off verification if RELAYCLIENT, DKIVERIFY and RELAYCLIENT_NODKVERIFY is set
 *
 * Revision 1.40  2013-08-18 15:52:51+05:30  Cprogrammer
 * revert back to default verification mode if both dksign, dkverify are not set
 *
 * Revision 1.39  2013-08-17 16:00:40+05:30  Cprogrammer
 * added case for duplicate DomainKey-Signature header
 *
 * Revision 1.38  2013-08-17 14:59:29+05:30  Cprogrammer
 * BUG - corrected location of private key when % sign is removed
 *
 * Revision 1.37  2013-01-24 22:42:07+05:30  Cprogrammer
 * alternate code for chosing DKSIGN selector filename
 *
 * Revision 1.36  2011-11-10 14:31:42+05:30  Cprogrammer
 * BUG ssout to be assigned only after pidopen
 *
 * Revision 1.35  2011-11-07 09:35:25+05:30  Cprogrammer
 * set ssout, sserr, ssin before executing other functions
 *
 * Revision 1.34  2011-07-29 09:28:48+05:30  Cprogrammer
 * fixed key file name
 *
 * Revision 1.33  2011-07-28 19:34:45+05:30  Cprogrammer
 * BUG - fixed opening of private key with absolute path
 *
 * Revision 1.32  2011-07-22 19:29:06+05:30  Cprogrammer
 * fixed compilation error
 *
 * Revision 1.31  2011-07-22 14:39:53+05:30  Cprogrammer
 * added DKDOMAIN feature to set d= tag
 *
 * Revision 1.30  2011-06-04 17:44:16+05:30  Cprogrammer
 * remove % sign from private key if file not found
 *
 * Revision 1.29  2010-07-21 08:59:14+05:30  Cprogrammer
 * use CONTROLDIR environment variable instead of a hardcoded control directory
 *
 * Revision 1.28  2009-08-13 18:36:39+05:30  Cprogrammer
 * for verification continue in case of DK_SYNTAX errors
 *
 * Revision 1.27  2009-04-22 13:42:47+05:30  Cprogrammer
 * made fd for custom error configurable through env variable ERROR_FD
 *
 * Revision 1.26  2009-04-21 09:05:14+05:30  Cprogrammer
 * return relevant error message for reading private key
 *
 * Revision 1.25  2009-04-21 08:56:03+05:30  Cprogrammer
 * return temp errors for temporary failures
 *
 * Revision 1.24  2009-04-21 08:15:39+05:30  Cprogrammer
 * moved stralloc variable outside block
 *
 * Revision 1.23  2009-04-20 22:20:40+05:30  Cprogrammer
 * added DKSIGNOPTIONS
 *
 * Revision 1.22  2009-04-07 11:38:02+05:30  Cprogrammer
 * use TMPDIR env variable for tmp directory
 *
 * Revision 1.21  2009-04-05 12:52:10+05:30  Cprogrammer
 * added preprocessor warning
 *
 * Revision 1.20  2009-03-31 08:21:12+05:30  Cprogrammer
 * set dksign when RELAYCLIENT is defined when both dksign, dkverify are undefined
 *
 * Revision 1.19  2009-03-28 22:26:35+05:30  Cprogrammer
 * use DKSIGN,DKVERIFY env variables if RELAYCLIENT is not set
 *
 * Revision 1.18  2009-03-28 22:02:32+05:30  Cprogrammer
 * removed extra white space
 *
 * Revision 1.17  2009-03-28 11:34:51+05:30  Cprogrammer
 * BUG fix. corrected setting of dksign, dkverify variables
 *
 * Revision 1.16  2009-03-22 16:58:13+05:30  Cprogrammer
 * report custom errors to qmail-queue through custom error interface
 *
 * Revision 1.15  2009-03-21 15:15:56+05:30  Cprogrammer
 * improved logic
 *
 * Revision 1.14  2009-03-20 22:35:24+05:30  Cprogrammer
 * fix for multi-line headers
 *
 * Revision 1.13  2009-03-19 08:28:12+05:30  Cprogrammer
 * added EXCLUDEHEADERS
 *
 * Revision 1.12  2009-03-14 17:11:32+05:30  Cprogrammer
 * added DK_STAT_GRANULARITY
 *
 * Revision 1.11  2009-03-14 08:52:54+05:30  Cprogrammer
 * look for domainkey in control/domainkeys
 *
 * Revision 1.10  2005-08-23 17:33:37+05:30  Cprogrammer
 * gcc 4 compliance
 *
 * Revision 1.9  2005-04-01 21:42:04+05:30  Cprogrammer
 * added DK_STAT_SYNTAX
 * changed error codes
 *
 * Revision 1.8  2004-11-02 09:15:53+05:30  Cprogrammer
 * commented out writing of Comments: header
 *
 * Revision 1.7  2004-10-24 21:32:00+05:30  Cprogrammer
 * removed unecessary header files
 *
 * Revision 1.6  2004-10-22 20:28:18+05:30  Cprogrammer
 * added RCS id
 *
 * Revision 1.5  2004-10-22 15:36:45+05:30  Cprogrammer
 * removed readwrite.h
 *
 * Revision 1.4  2004-10-22 14:44:26+05:30  Cprogrammer
 * use control_readnativefile to avoid skipping signure with '#' char
 *
 * Revision 1.3  2004-10-20 20:08:53+05:30  Cprogrammer
 * libdomainkeys-0.62
 *
 * Revision 1.2  2004-09-23 22:55:32+05:30  Cprogrammer
 * removed uneccessary header files
 *
 * Revision 1.1  2004-08-28 01:02:16+05:30  Cprogrammer
 * Initial revision
 *
 */
#ifdef DOMAIN_KEYS
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sgetopt.h>
#include <substdio.h>
#include <open.h>
#include <sig.h>
#include <fmt.h>
#include <fd.h>
#include <alloc.h>
#include <str.h>
#include <getln.h>
#include <error.h>
#include <case.h>
#include <stralloc.h>
#include <datetime.h>
#include <now.h>
#include <wait.h>
#include <env.h>
#include <scan.h>
#include <mess822.h>
#include <makeargs.h>
#include "qmail.h"
#include "control.h"
#include "variables.h"
#include "domainkeys.h"
#include "qmulti.h"
#include "auto_control.h"
#include "pidopen.h"

#define DEATH 86400	/*- 24 hours; _must_ be below q-s's OSSIFIED (36 hours) */
#define ADDR 1003
#define ADVICE_BUF 2048

char            inbuf[2048];
struct substdio ssin;
char            outbuf[256];
struct substdio ssout;
struct substdio sserr;
char            errbuf[256];

datetime_sec    starttime;
struct datetime dt;
unsigned long   uid;
int             readfd;
char           *dksign = 0;
char           *dkverify = 0;

void
die(e)
	int             e;
{
	_exit(e);
}

void
die_write()
{
	die(53);
}

void
die_read()
{
	die(54);
}

void
sigalrm()
{
	/*- thou shalt not clean up here */
	die(52);
}

void
sigbug()
{
	die(81);
}

void
custom_error(char *flag, char *status, char *code)
{
	char           *c;

	if (substdio_put(&sserr, flag, 1) == -1)
		die_write();
	if (substdio_put(&sserr, "qmail-dk: ", 10) == -1)
		die_write();
	if (substdio_puts(&sserr, status) == -1)
		die_write();
	if (code) {
		if (substdio_put(&sserr, " (#", 3) == -1)
			die_write();
		c = (*flag == 'Z') ? "4" : "5";
		if (substdio_put(&sserr, c, 1) == -1)
			die_write();
		if (substdio_put(&sserr, code + 1, 4) == -1)
			die_write();
		if (substdio_put(&sserr, ")", 1) == -1)
			die_write();
	}
	if (substdio_flush(&sserr) == -1)
		die_write();
	return;
}

void
maybe_die_dk(e)
	DK_STAT         e;
{
	switch (e)
	{
	case DK_STAT_NORESOURCE:
		_exit(51);
	case DK_STAT_INTERNAL:
		_exit(81);
	case DK_STAT_ARGS:
		custom_error("Z", "Arguments are not usable. (#4.3.5)", 0);
		_exit(88);
	case DK_STAT_SYNTAX:
		if (!dksign && dkverify)
			return;
		custom_error("Z", "Message is not valid syntax. Signature could not be created/checked (#4.6.0)", 0);
		_exit(88);
	case DK_STAT_CANTVRFY:
		custom_error("Z", "Cannot get domainkeys to verify signature (#5.7.5)", 0);
		_exit(88);
	case DK_STAT_BADKEY:
		if (env_get("DKVERIFY"))
			custom_error("Z", "Unusable public key for verifying (#5.7.5)", 0);
		else
			custom_error("Z", "Unusable private key for signing (#5.7.15", 0);
		_exit(88);
	default:
		return;
	}
}

char            tmp[FMT_ULONG];
DK_LIB         *dklib;
DK             *dk;
DK_STAT         st;
stralloc        dkoutput = { 0 };    /*- Domainkey-Signature */
stralloc        dksignature = { 0 }; /*- content of private signature */
stralloc        dkopts = { 0 };
char           *dkexcludeheaders;

static void
write_signature(DK *dka, char *dk_selector, char *keyfn,
	int advicelen, int opth, char *canon)
{
	unsigned char   advice[ADVICE_BUF];
	char           *selector, *from, *ptr;
	static stralloc keyfnfrom = { 0 };
	int             i;

	from = dk_from(dka);
	if (keyfn[0] != '/') {
		if (!controldir) {
			if (!(controldir = env_get("CONTROLDIR")))
				controldir = auto_control;
		}
		if (!stralloc_copys(&keyfnfrom, controldir) ||
				!stralloc_append(&keyfnfrom, "/"))
			die(51);
	}
	i = str_chr(keyfn, '%');
	if (keyfn[i]) {
		if (keyfn[0] == '/') {
			if (!stralloc_copyb(&keyfnfrom, keyfn, i))
				die(51);
		} else
		if (!stralloc_catb(&keyfnfrom, keyfn, i) ||
				!stralloc_cats(&keyfnfrom, from) ||
				!stralloc_cats(&keyfnfrom, keyfn + i + 1) ||
				!stralloc_0(&keyfnfrom))
			die(51);
		if (access(keyfnfrom.s, F_OK)) {
			if (errno != error_noent) {
				custom_error("Z", "Unable to read private key. (#4.3.0)", 0);
				_exit(88);
			}
			/*- since file is not found remove '%' sign */
			keyfnfrom.len = 8;
			if (keyfn[0] == '/') {
				if (!stralloc_copyb(&keyfnfrom, keyfn, i))
					die(51);
			} else
			if (!stralloc_catb(&keyfnfrom, keyfn, i))
				die(51);
			if ((i - 1) > 0 && keyfn[i - 1] == '/' && keyfn[i + 1] == '/')
				i++;
			if (!stralloc_cats(&keyfnfrom, keyfn + i + 1) ||
					!stralloc_0(&keyfnfrom))
				die(51);
		} 
	} else {
		if (keyfn[0] == '/') {
			if (!stralloc_copys(&keyfnfrom, keyfn))
				die(51);
		} else
		if (!stralloc_cats(&keyfnfrom, keyfn))
			die(51);
		if (!stralloc_0(&keyfnfrom))
			die(51);
	}
	switch (control_readnativefile(&dksignature, keyfn[0] == '/' ? keyfnfrom.s : keyfnfrom.s + 8, 1))
	{
	case 0: /*- file not present */
		/*
		 * You may have multiple domains, but may chose to sign
		 * only for few domains which have the key present. Do not
		 * treat domains with missing key as an error.
		 */
		if (keyfn[i])
			return;
		die(35);
	case 1:
		break;
	default:
		custom_error("Z", "Unable to read private key. (#4.3.0)", 0);
		_exit(88);
	}
	for (i = 0; i < dksignature.len; i++) {
		if (dksignature.s[i] == '\0')
			dksignature.s[i] = '\n';
	}
	if (!stralloc_0(&dksignature))
		die(51);
	st = dk_getsig(dka, dksignature.s, advice, advicelen);
	maybe_die_dk(st);
	if (!dk_selector) {
		selector = keyfn;
		while (*keyfn) {
			if (*keyfn == '/')
				selector = keyfn + 1;
			keyfn++;
		}
	} else
		selector = dk_selector;
	if (!stralloc_cats(&dkoutput,
#if 0
		"Comment: DomainKeys? See http://antispam.yahoo.com/domainkeys\n"
#endif
		"DomainKey-Signature: a=rsa-sha1; q=dns; c=") ||
			!stralloc_cats(&dkoutput, canon) ||
			!stralloc_cats(&dkoutput, ";\n") ||
			!stralloc_cats(&dkoutput, "    s=") ||
			!stralloc_cats(&dkoutput, selector) ||
			!stralloc_cats(&dkoutput, "; d="))
		die(51);
	ptr = env_get("DKDOMAIN");
	if (from || ptr) {
		if (!stralloc_cats(&dkoutput, ptr ? ptr : from))
			die(51);
	} else
	if (!stralloc_cats(&dkoutput, "unknown") ||
			!stralloc_cats(&dkoutput, ";\n") ||
			!stralloc_cats(&dkoutput, "    b=") ||
			!stralloc_cats(&dkoutput, (char *) advice))
		die(51);
	if (dkexcludeheaders || opth) {
		if ((i = dk_headers(dka, NULL)) > 0) {
			if (!(ptr = alloc(i)))
				die(51);
			if (!dk_headers(dka, ptr))
				die(51);
			if (!stralloc_cats(&dkoutput, ";\n    h=") ||
					!stralloc_cats(&dkoutput, ptr))
				die(51);
			alloc_free(ptr);
		}
	}
	if (!stralloc_cats(&dkoutput, ";\n"))
		die(51);
}

int
find_header(stralloc *line)
{
	static stralloc headers = { 0 };
	int             n, i, j;

	for (n = 0; n < line->len; ++n) {
		if (line->s[n] == ':')
			break;
	}
	if (n == line->len)
		return -1;
	if (!headers.len) {
		if (!stralloc_copys(&headers, ""))
			die(51);
		if (dkexcludeheaders) {
			if (!stralloc_cats(&headers, dkexcludeheaders) ||
					!stralloc_append(&headers, ":"))
				die(51);
		}
	}
	if (!headers.len)
		return 0;
	for (i = j = 0; i < headers.len; ++i) {
		if (headers.s[i] != ':')
			continue;
		if (i - j == n && !case_diffb(headers.s + j, n, line->s))
			return 1;
		j = i + 1;
	}
	return 0;
}

int
dk_setoptions(char **selector, int *advicelen, int *opth, int *optr, int *optc,
	char **canon, char *signOptions)
{
	char          **argv;
	int             ch, argc;

	*opth = 0;
	*optr = 0;
	*optc = DK_CANON_NOFWS;
	*canon = "nofws";
	*selector = 0;
	if (!signOptions)
		return (0);
	if (!stralloc_copys(&dkopts, "qmail-dk ") ||
			!stralloc_cats(&dkopts, signOptions) ||
			!stralloc_0(&dkopts) ||
			!(argv = makeargs(dkopts.s)))
		die(51);
	for (argc = 0;argv[argc];argc++);
	while ((ch = sgopt(argc, argv, "hrb:c:s:")) != sgoptdone) {
		switch (ch)
		{
		case 'h':
			*opth = 1;
			break;
		case 'r':
			*optr = 1;
			*opth = 1;
			break;
		case 'b':
			*advicelen = atoi(optarg);
			if (*advicelen > ADVICE_BUF)
				*advicelen = ADVICE_BUF;
			break;
		case 'c':
			if (!str_diffn("simple\0", optarg, 7)) {
				*optc = DK_CANON_SIMPLE;
				*canon = "simple";
			}
			break;
		case 's':
			*selector = optarg;
			break;
		default:
			free_makeargs(argv);
			return (1);
		}
	} /*- while ((ch = sgopt(argc, argv, "hrb:c:s:")) != sgoptdone) */
	free_makeargs(argv);
	return (0);
}

int
main(int argc, char *argv[])
{
	int             errfd, pim[2];
	int             wstat, match, opth = 0, optr = 0, optc = DK_CANON_NOFWS,
					advicelen = ADVICE_BUF, ret;
	char           *x, *relayclient, *canon = "nofws", *selector = 0; 
	stralloc        line = {0}, dkfn = {0};
	unsigned long   pid;

	sig_blocknone();
	umask(033);
	if (!(x = env_get("ERROR_FD")))
		errfd = CUSTOM_ERR_FD;
	else
		scan_int(x, &errfd);
	substdio_fdbuf(&sserr, write, errfd, errbuf, sizeof(errbuf));
	dksign = env_get("DKSIGN");
	dkverify = env_get("DKVERIFY");
	relayclient = (env_get("RELAYCLIENT") || env_get("AUTHINFO")) ? "" : 0;
	if (dkverify && relayclient && env_get("RELAYCLIENT_NODKVERIFY"))
		return (qmulti("DKQUEUE", argc, argv));
	if (!dksign && !dkverify && relayclient) {
		if (!(dksign = env_get("DKKEY"))) {
			if (!stralloc_copys(&dkfn, "domainkeys/%/default") ||
					!stralloc_0(&dkfn))
				die(51);
			dksign = dkfn.s;
		}
	}
	if (!(dklib = dk_init(&st))) {
		maybe_die_dk(st);
		custom_error("Z", "dk initialization failed (#4.3.0)", 0);
		_exit(88);
	}
	/*- Initialization */
	if (dksign) {
		if (dk_setoptions(&selector, &advicelen, &opth, &optr, &optc, &canon, env_get("DKSIGNOPTIONS"))) {
			custom_error("Z", "Invalid DKSIGNOPTIONS (#4.3.0)", 0);
			_exit(88);
		}
		if (!(dk = dk_sign(dklib, &st, optc))) {
			maybe_die_dk(st);
			custom_error("Z", "dk_sign failed (#4.3.0)", 0);
			_exit(88);
		}
		if (optr && dk_setopts(dk, DKOPT_RDUPE) != DK_STAT_OK) {
			custom_error("Z", "DKOPT_RDUPE failed (#4.3.0)", 0);
			_exit(88);
		}
	} else
	if (dkverify) {
		if (!(dk = dk_verify(dklib, &st))) {
			maybe_die_dk(st);
			custom_error("Z", "dk_verify failed (#4.3.0)", 0);
			_exit(88);
		}
	}
	uid = getuid();
	starttime = now();
	datetime_tai(&dt, starttime);
	sig_pipeignore();
	sig_miscignore();
	sig_alarmcatch(sigalrm);
	sig_bugcatch(sigbug);
	alarm(DEATH);
	if (!(x = env_get("TMPDIR")))
		x = "/tmp";
	if ((ret = pidopen(starttime, x)))
		die(ret);
	if ((readfd = open_read(pidfn)) == -1)
		die(63);
	if (unlink(pidfn) == -1)
		die(63);
	substdio_fdbuf(&ssout, write, messfd, outbuf, sizeof(outbuf));
	substdio_fdbuf(&ssin, read, 0, inbuf, sizeof(inbuf)); /*- message content */
	dkexcludeheaders = env_get("DKEXCLUDEHEADERS");
	if (dkexcludeheaders) {
		int             hdr_continue, in_header = 1;

		hdr_continue = 0;
		for (;;) {
	
			if (getln(&ssin, &line, &match, '\n') == -1)
				die_read();
			if (!match && line.len == 0)
				break;
			if (substdio_put(&ssout, line.s, line.len) == -1)
				die_write();
			if (!dksign && !dkverify)
				continue;
			if (in_header && !mess822_ok(&line))
				in_header = 0;
			if (in_header) {
				if (line.s[0] == ' ' || line.s[0] == '\t') {
					if (hdr_continue)
						continue;
				} else
				if (find_header(&line) == 1) {
					hdr_continue = 1;
					continue;
				} else
					hdr_continue = 0;
			}
			if (match) {
				st = dk_message(dk, (unsigned char *) line.s, line.len - 1);
				maybe_die_dk(st);
				st = dk_message(dk, (unsigned char *) "\r\n", 2);
			} else
				st = dk_message(dk, (unsigned char *) line.s, line.len);
			maybe_die_dk(st);
		}
	} else
	for (;;) {
		register int    n;
		register char  *t;
		int             i;

		if ((n = substdio_feed(&ssin)) < 0)
			die_read();
		if (!n)
			break;
		t = substdio_PEEK(&ssin);
		if (dksign || dkverify) {
			for (i = 0; i < n; i++) {
				if (t[i] == '\n')
					st = dk_message(dk, (unsigned char *) "\r\n", 2);
				else
					st = dk_message(dk, (unsigned char *) t + i, 1);
				maybe_die_dk(st);
			}
		}
		if (substdio_put(&ssout, t, n) == -1)
			die_write();
		substdio_SEEK(&ssin, n);
	}
	if (substdio_flush(&ssout) == -1)
		die_write();
	if (dksign || dkverify) {
		if (dksign)
			write_signature(dk, selector, dksign, advicelen, opth, canon);
		else
		if (dkverify) {
			char           *status = 0, *code = 0;

			if (!stralloc_copys(&dkoutput, "DomainKey-Status: "))
				die(51);
			switch ((st = dk_eom(dk, (void *) 0)))
			{
			case DK_STAT_OK:
				status = "good        ";
				break;
			case DK_STAT_BADSIG:
				status = "bad         ";
				code = "X.7.5";
				break;
			case DK_STAT_NOSIG:
				status = "no signature";
				code = "X.7.5";
				break;
			case DK_STAT_NOKEY:
				status = "no key      ";
				code = "X.7.0";
				break;
			case DK_STAT_CANTVRFY:
				status = "DK_STAT_CANTVRFY: Cannot get domain key to verify signature";
				code = "X.7.0";
				break;
			case DK_STAT_BADKEY:
				status = "bad key     ";
				code = "X.7.5";
				break;
			case DK_STAT_INTERNAL:
				status = "bad format  ";
				code = "X.3.0";
				break;
			case DK_STAT_ARGS:
				status = "bad format  ";
				code = "X.3.5";
				break;
			case DK_STAT_SYNTAX:
				status = "bad format  ";
				code = "X.6.0";
				break;
			case DK_STAT_NORESOURCE:
				status = "no resources";
				code = "X.3.0";
				break;
			case DK_STAT_REVOKED:
				status = "revoked     ";
				code = "X.7.5";
				break;
			case DK_STAT_GRANULARITY:
				status = "bad sender  ";
				code = "X.7.5";
				break;
			case DK_STAT_DUPLICATE:
				status = "dup sig     ";
				code = "X.6.0";
				break;
			}
			if (!stralloc_cats(&dkoutput, status) ||
					!stralloc_cats(&dkoutput, "\n"))
				die(51);
			if (dkverify[str_chr(dkverify, 'A' + st)]) {
				custom_error("D", status, code); /*- return permanent error */
				die(88);
			}
			if (dkverify[str_chr(dkverify, 'a' + st)]) {
				custom_error("Z", status, code); /*- return temporary error */
				die(88);
			}
		}
	}
	if (pipe(pim) == -1)
		die(59);
	switch (pid = vfork())
	{
	case -1:
		close(pim[0]);
		close(pim[1]);
		die(58);
	case 0:
		close(pim[1]);
		if (fd_move(0, pim[0]) == -1)
			die(120);
		return (qmulti("DKQUEUE", argc, argv));
	}
	close(pim[0]);
	substdio_fdbuf(&ssin, read, readfd, inbuf, sizeof(inbuf));
	substdio_fdbuf(&ssout, write, pim[1], outbuf, sizeof(outbuf));
	if (substdio_bput(&ssout, dkoutput.s, dkoutput.len) == -1) /*- write DK signature */
		die_write();
	switch (substdio_copy(&ssout, &ssin))
	{
	case -2:
		die_read();
	case -3:
		die_write();
	}
	if (substdio_flush(&ssout) == -1)
		die_write();
	close(pim[1]);
	if (wait_pid(&wstat, pid) != pid)
		die(57);
	if (wait_crashed(wstat))
		die(57);
	die(wait_exitcode(wstat));
	/*- Not Reached */
	exit(0);
}
#else
#warning "not compiled with -DDOMAIN_KEYS"
#include "substdio.h"
#include <unistd.h>

static char     sserrbuf[512];
struct substdio sserr;

int
main(argc, argv)
	int             argc;
	char          **argv;
{
	substdio_fdbuf(&sserr, write, 2, sserrbuf, sizeof(sserrbuf));
	substdio_puts(&sserr, "not compiled with -DDOMAIN_KEYS\n");
	substdio_flush(&sserr);
	_exit(111);
}
#endif

#ifndef lint
void
getversion_qmail_dk_c()
{
	static char    *x = "$Id: qmail-dk.c,v 1.54 2021-06-15 22:14:47+05:30 Cprogrammer Exp mbhangui $";

#ifdef DOMAIN_KEYS
	x = sccsidmakeargsh;
	x = sccsidqmultih;
	x = sccsidpidopenh;
#endif
	x++;
}
#endif
