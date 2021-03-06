# chkconfig: 345 14 91
# description: Starts svscan system and associated services
### BEGIN INIT INFO
# Provides:          svscan
# Required-Start:    $network
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Description:       Starts svscan system and associated services
# Short-Description: Start/Stop svscan
### END INIT INFO
#
# $Id: qmailctl.sh,v 1.67 2020-11-29 17:53:54+05:30 Cprogrammer Exp mbhangui $
#
#
SERVICE=@servicedir@
#
#
#
if [ -f /etc/lsb-release -o -f /etc/debian_version ] ; then
	SYSTEM=Debian
elif [ -f /etc/SuSE-release ] ; then
	SYSTEM=SuSE
else
	if [ -f /etc/os-release ] ; then
		NAME=`grep -w NAME /etc/os-release | cut -d= -f2 | sed -e 's{"{{g'`
		case "$NAME" in
			"openSUSE Tumbleweed")
			SYSTEM=SuSE
			;;
			*)
			SYSTEM=`uname -s`
			;;
		esac
	else
		SYSTEM=`uname -s`
	fi
fi
ECHO=echo
case "$SYSTEM" in
	Darwin*|Debian|SuSE|FreeBSD)
		RES_COL=60
		MOVE_TO_COL="$ECHO -en \\033[${RES_COL}G"
		SETCOLOR_SUCCESS="$ECHO -en \\033[1;32m"
		SETCOLOR_FAILURE="$ECHO -en \\033[1;31m"
		SETCOLOR_WARNING="$ECHO -en \\033[1;33m"
		SETCOLOR_NORMAL="$ECHO -en \\033[0;39m"
		;;
esac
if [ -f /etc/indimail/indimail-mta-release ] ; then
	have_qmail=1
else
	have_qmail=0
fi

myecho_success() {
	$MOVE_TO_COL
	$ECHO -n "["
	$SETCOLOR_SUCCESS
	$ECHO -n "  OK  "
	$SETCOLOR_NORMAL
	$ECHO -n "]"
	$ECHO -ne "\r"
	return 0
}

myecho_failure() {
	$MOVE_TO_COL
	$ECHO -n "["
	$SETCOLOR_FAILURE
	$ECHO -n $"FAILED"
	$SETCOLOR_NORMAL
	$ECHO -n "]"
	$ECHO -ne "\r"
	return 1
}

noqmail() {
	if [ $have_qmail -ne 1 ] ; then
		echo "Usage: `basename $0` {start|stop|restart|shut|stat|rotate|help}"
		myhelp
		exit 1
	fi
}

# Log that something succeeded
mysuccess() {
	myecho_success
	return 0
}

# Log that something failed
myfailure() {
	local rc=$?
	myecho_failure
	return $rc
}

# Check that we're a privileged user
[ `id -u` = 0 ] || exit 4
# Source function library.
if [ -f /etc/rc.d/init.d/functions ] ; then
	. /etc/rc.d/init.d/functions
elif [ -f /etc/init.d/functions ] ; then
	. /etc/init.d/functions
elif [ -f /lib/lsb/init-functions ] ; then
	. /lib/lsb/init-functions
fi
case "$SYSTEM" in
	Darwin*)
	. /etc/rc.common
	CheckForNetwork
	if [ "${NETWORKUP}" != "-YES-" ]
	then
		exit 0
	fi
	succ=mysuccess
	fail=myfailure
	;;
	Debian|SuSE)
	succ=mysuccess
	fail=myfailure
	;;
	LINUX)
	succ=success
	fail=failure
	# Get config.
	if [ -f /etc/sysconfig/network ] ; then
		. /etc/sysconfig/network
		# Check that networking is up.
		if [ "${NETWORKING}" = "no" ] ; then
			exit 0
		fi
	else # FC19, FC20, ...
		NETWORKING=yes
	fi
	;;
	*)
	succ=mysuccess
	fail=myfailure
	;;
esac

if [ -x /usr/bin/systemctl ] ; then
	/usr/bin/systemctl is-enabled svscan > /dev/null 2>&1
	if [ $? -ne 0 ] ; then
		if [ -f @prefix@/sbin/initsvc ] ; then
			@prefix@/sbin/initsvc -on > /dev/null && $succ || $fail
			$ECHO -ne "\n"
		fi
		/usr/bin/systemctl is-enabled svscan > /dev/null 2>&1
	fi
	if [ $? -ne 0 ] ; then
		SYSTEMCTL_SKIP_REDIRECT=1
		if [ -f /etc/rc.d/init.d/functions ] ; then
			. /etc/rc.d/init.d/functions
		elif [ -f /etc/init.d/functions ] ; then
			. /etc/init.d/functions
		elif [ -f /lib/lsb/init-functions ] ; then
			. /lib/lsb/init-functions
		fi
		$ECHO -n $"svscan is disabled: "
		$fail
		$ECHO -ne "\n"
		exit 1
	fi
fi

PATH=$PATH:@prefix@/bin:@prefix@/sbin:/usr/bin:/bin
export PATH
myhelp()
{
if [ $qmail -eq 1 ] ; then
/bin/cat <<HELP
  start -- starts mail service (smtp connection accepted, mail can go out)
   stop -- stops mail service (smtp connections refused, nothing goes out)
restart -- stops and restarts smtp, sends qmail-send a TERM & restarts it
   kill -- Send kill signal to tcpserver, supvervise, qmail-send processes.
   shut -- Shutdown entire svscan service.
  flush -- Schedules queued messages for immediate delivery
 reload -- sends qmail-send HUP, rereading locals and virtualdomains
 status -- status of svscan process & IndiMail services
 rotate -- rotate all logfiles (sending ALRM to multilog)
  queue -- shows status of queue
  pause -- temporarily stops mail service (connections accepted, nothing goes out)
   cont -- continues paused mail service
    cdb -- rebuild the tcpserver cdb file for smtp, qmtp, qmqp, imap, pop3 and poppass
HELP
else
/bin/cat <<HELP
  start -- starts svscan service
   stop -- stops  svscan service
restart -- stops and restarts svscan service
   shut -- Shutdown entire svscan service.
 status -- status of svscan process and configured services
 rotate -- rotate all logfiles (sending ALRM to multilog)
HELP
fi
}

stop()
{
	if [ -d /var/lock/subsys -a ! -f /var/lock/subsys/svscan ] ; then
		exit 0
	fi
	local ret=0
	case "$SYSTEM" in
		FreeBSD)
		$ECHO -n $"Stopping svscan: "
		if [ -d /run ] ; then
			sv_pid=/run/svscan/.svscan.pid
			daemon_pid=/run/sv_daemon.pid
		elif [ -d /var/run ] ; then
			sv_pid=/var/run/svscan/.svscan.pid
			daemon_pid=/var/run/sv_daemon.pid
		else
			sv_pid=/tmp/svscan.pid
			daemon_pid=/tmp/sv_daemon.pid
		fi
		if [ ! -f $sv_pid -a ! -f $daemon_pid ] ; then
			$succ
			echo
			return $ret
		fi
		;;
	esac
	for i in `echo $SERVICE/*`
	do
		$ECHO -n $"Stopping $i: "
		@prefix@/bin/svc -d $i 2>>/tmp/sv.err && $succ || $fail
		RETVAL=$?
		echo
		let ret+=$RETVAL
	done
	if [ ! -f /usr/bin/systemctl ] ; then
		$ECHO -n $"Stopping svscan: "
		if [ -f /sbin/initctl ] ; then
			/sbin/initctl stop svscan >/dev/null 2>>/tmp/sv.err && $succ || $fail
		elif [ -f /usr/sbin/daemon ] ; then
			if [ -f /var/run/sv_daemon.pid ] ; then
				kill `cat /var/run/sv_daemon.pid` && $succ || $fail
			fi
		elif [ -f /etc/inittab ] ; then
  			grep "^SV:" /etc/inittab | grep svscan | grep respawn >/dev/null 2>&1
			if [ $? -ne 0 ]; then
				case "$SYSTEM" in
					FreeBSD)
					/usr/bin/killall -c svscan && $succ || $fail
					;;
					*)
					/usr/bin/killall -e -w svscan && $succ || $fail
					;;
				esac
			else
				RETVAL=0
			fi
		else
			/usr/bin/killall svscan && $succ || $fail
		fi
		RETVAL=$?
		echo
		if [ -s /tmp/sv.err ] ; then
			/bin/cat /tmp/sv.err
		fi
		/bin/rm -f /tmp/sv.err
	fi
	if [ -d /var/lock/subsys ] ; then
		[ $ret -eq 0 ] && rm -f /var/lock/subsys/svscan
	fi
	return $ret
}

start()
{
	local ret=0
	if [ -d /var/lock/subsys -a -f /var/lock/subsys/svscan ] ; then
		exit 0
	fi
	@prefix@/bin/svstat $SERVICE/.svscan/log > /dev/null
	if [ $? -ne 0 ] ; then
		$ECHO -n $"Starting svscan: "
		if [ -f /sbin/initctl ] ; then
			/sbin/initctl start svscan >/dev/null 2>>/tmp/sv.err && $succ || $fail
		elif [ -f /usr/sbin/daemon ] ; then
			if [ -d /run ] ; then
				daemon_pid=/run/sv_daemon.pid
			elif [ -d /var/run ] ; then
				daemon_pid=/var/run/sv_daemon.pid
			else
				daemon_pid=/tmp/sv_daemon.pid
			fi
			env SETSID=1 /usr/sbin/daemon -cS -P $daemon_pid -R 5 \
				-t "$SYSTEM"_svscan @libexecdir@/svscanboot && $succ || $fail
		else
			if [ -w /dev/console ] ; then
				device=/dev/console
			else
				device=/dev/null
			fi
			grep "^SV:" /etc/inittab | grep svscan | grep respawn >/dev/null 2>&1
			if [ $? -ne 0 ]; then
				grep -v "svscan" /etc/inittab > /etc/inittab.qmailctl.$$ 2>&1
				if [ " $SYSTEM" = " Debian" ] ; then
					echo "SV:2345:respawn:@prefix@/sbin/svscanboot $SERVICE <>$device 2<>$device" >> /etc/inittab.qmailctl.$$
				else
					echo "SV:345:respawn:@prefix@/sbin/svscanboot $SERVICE <>$device 2<>$device" >> /etc/inittab.qmailctl.$$
				fi
				if [ $? -eq 0 ] ; then
					/bin/mv /etc/inittab.qmailctl.$$ /etc/inittab
				fi
				/sbin/init q
			fi
		fi
		RETVAL=$?
		echo
		let ret+=$RETVAL
	else
		for i in `echo $SERVICE/*`
		do
			if [ ! -f $i/down ] ; then
				$ECHO -n $"Starting $i: "
				@prefix@/bin/svc -u $i 2>/tmp/sv.err && $succ || $fail
				RETVAL=$?
				echo
				let ret+=$RETVAL
			fi
		done
	fi
	if [ -s /tmp/sv.err ] ; then
		/bin/cat /tmp/sv.err
	fi
	/bin/rm -f /tmp/sv.err
	if [ -d /var/lock/subsys ] ; then
		[ $ret -eq 0 ] && touch /var/lock/subsys/svscan
	fi
	return $ret
}

case "$1" in
	stop)
		stop
		[ $? -eq 0 ] && exit 0 || exit 1
		;;
		start)
		start
		[ $? -eq 0 ] && exit 0 || exit 1
		;;
	restart|force-reload)
		stop
		RETVAL=$?
		let ret+=$RETVAL
		start
		RETVAL=$?
		let ret+=$RETVAL
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	condrestart|try-restart)
		noqmail
		ret=0
		for i in `echo $SERVICE/qmail-smtpd.* $SERVICE/qmail-qmqpd.* $SERVICE/qmail-qmtpd.*`
		do
			$ECHO -n $"Stopping $i: "
			@prefix@/bin/svc -d $i && $succ || $fail
			RETVAL=$?
			echo
			let ret+=$RETVAL
		done
		for i in `echo $SERVICE/qmail-send.*`
		do
			$ECHO -n $"Terminating $i: "
			@prefix@/bin/svc -t $i && $succ || $fail
			RETVAL=$?
			echo
			let ret+=$RETVAL
		done
		for i in `echo $SERVICE/qmail-smtpd.* $SERVICE/qmail-qmqpd.* $SERVICE/qmail-qmtpd.*`
		do
			if [ ! -f $i/down ] ; then
			$ECHO -n $"Starting $i: "
				@prefix@/bin/svc -u $i && $succ || $fail
				RETVAL=$?
				echo
				let ret+=$RETVAL
			fi
		done
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	shut)
		$ECHO -n $"shutdown svscan: "
		if [ -f /sbin/initctl ] ; then
			/sbin/initctl stop svscan >/dev/null && $succ || $fail
		elif [ -f /bin/systemctl ] ; then
			/bin/systemctl stop svscan && $succ || $fail
		else
			/usr/bin/killall -e -w svscan && $succ || $fail
		fi
		ret=$?
		echo
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	kill)
		noqmail
		$ECHO -n $"killing tcpserver,supervise,qmail-send: "
		kill `ps -ef| egrep "tcpserver|supervise|qmail-send" | grep -v grep | awk '{print $2}'` && $succ || $fail
		ret=$?
		echo
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	rotate)
		ret=0
		for i in `echo $SERVICE/* $SERviCE/.svscan`
		do
			if [ -d $i/log ] ; then
				$ECHO -n $"Rotating $i: "
				@prefix@/bin/svc -a $i/log && $succ || $fail
				RETVAL=$?
				echo
				let ret+=$RETVAL
			fi
		done
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	flush)
		noqmail
		ret=0
		$ECHO -n $"Flushing timeout table + ALRM signal to qmail-send."
		@prefix@/sbin/qmail-tcpok > /dev/null && $succ || $fail
		echo
		for i in `echo $SERVICE/qmail-send.*`
		do
			$ECHO -n $"Flushing $i: "
			@prefix@/bin/svc -a $i && $succ || $fail
			RETVAL=$?
			echo
			let ret+=$RETVAL
		done
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	status|stat)
		ret=0
		if [ -x /sbin/initctl ] ; then
			/sbin/initctl status svscan
		else
			ps -ef| grep svscanboot| grep -v grep
		fi
		RETVAL=$?
		@prefix@/bin/svstat $SERVICE/.svscan/log $SERVICE/* $SERVICE/*/log
		let ret+=$RETVAL
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	queue)
		noqmail
		@prefix@/bin/qmail-qread -c
		ret=$?
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	reload)
		noqmail
		ret=0
		for i in `echo $SERVICE/qmail-send.*`
		do
			$ECHO -n $"sending HUP signal to $i: "
			@prefix@/bin/svc -h $i && $succ || $fail
			RETVAL=$?
			echo
			let ret+=$RETVAL
		done
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	pause)
		noqmail
		ret=0
		for i in `echo $SERVICE/qmail-send.* $SERVICE/qmail-smtpd.*`
		do
			$ECHO -n $"pausing $i: "
			@prefix@/bin/svc -p $i && $succ || $fail
			RETVAL=$?
			echo
			let ret+=$RETVAL
		done
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	cont)
		noqmail
		ret=0
		for i in `echo $SERVICE/qmail-send.* $SERVICE/qmail-smtpd.*`
		do
			$ECHO -n $"continuing $i: "
			@prefix@/bin/svc -c $i && $succ || $fail
			RETVAL=$?
			echo
			let ret+=$RETVAL
		done
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	cdb)
		noqmail
		ret=0
		for i in smtp qmtp qmqp imap pop3 poppass
		do
			for j in `/bin/ls @sysconfdir@/tcp*.$i.cdb 2>/dev/null`
			do
				t_file=`echo $j | cut -d. -f1,2`
				if [ ! -f $t_file ] ; then
					$ECHO -n $"deleting $j: "
					/bin/rm -f $j && $succ || $fail
					RETVAL=$?
					echo
					let ret+=$RETVAL
				fi
			done
			for j in `/bin/ls @sysconfdir@/tcp/tcp*.$i 2>/dev/null`
			do
				t1=`date +'%s' -r $j`
				if [ -f $j.cdb ] ; then
					t2=`date +'%s' -r $j.cdb`
				else
					t2=0
				fi
				if [ $t1 -gt $t2 ] ; then
					$ECHO -n $"building $j.cdb: "
					@prefix@/bin/tcprules $j.cdb $j.tmp < $j && /bin/chmod 664 $j.cdb \
						&& /bin/chown indimail:indimail $j.cdb && $succ || $fail
					RETVAL=$?
					echo
				else
					RETVAL=0
				fi
				let ret+=$RETVAL
			done
		done
		[ $ret -eq 0 ] && exit 0 || exit 1
		;;
	help)
		myhelp
		exit 0
		;;
	*)
		if [ $have_qmail -eq 1 ] ; then
			echo "Usage: `basename $0` {start|stop|condrestart|restart|shut|kill|flush|rotate|reload|stat|queue|pause|cont|cdb|help}"
		else
			echo "Usage: `basename $0` {start|stop|restart|shut|stat|rotate|help}"
		fi
		myhelp
		exit 1
	;;
esac
exit 0
