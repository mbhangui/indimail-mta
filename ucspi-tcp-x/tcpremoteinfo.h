/*
 * $Log: tcpremoteinfo.h,v $
 * Revision 1.3  2021-05-12 21:04:57+05:30  Cprogrammer
 * define arguments as array subscripts to fix gcc 11 warnings
 *
 * Revision 1.2  2005-06-10 09:13:36+05:30  Cprogrammer
 * added ipv6 support
 *
 * Revision 1.1  2003-12-31 19:57:33+05:30  Cprogrammer
 * Initial revision
 *
 */
#ifndef REMOTEINFO_H
#define REMOTEINFO_H

#include "stralloc.h"
#include "uint16.h"
#include "haveip6.h"
#ifdef LIBC_HAS_IP6
#include "uint32.h"
#endif

int             remoteinfo(stralloc *, char ip[4], uint16, char iplocal[4], uint16, unsigned int);
#ifdef LIBC_HAS_IP6
int             remoteinfo6(stralloc *, char ip[16], uint16, char iplocal[16], uint16, unsigned int, uint32);
#endif

#endif
