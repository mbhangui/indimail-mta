#
# $Log: rmail.sh,v $
# Revision 1.1  2010-06-19 13:34:26+05:30  Cprogrammer
# Initial revision
#
#
# Dummy UUCP rmail command for postfix/qmail systems

SENDMAIL="QMAIL/sbin/sendmail"
IFS=" " read junk from junk junk junk junk junk junk junk relay

case "$from" in
 *[@!]*) ;;
      *) from="$from@$relay";;
esac

exec $SENDMAIL -i -f "$from" -- "$@"
