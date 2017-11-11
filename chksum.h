/**************************************************************************
$Header: chksum.h Sun 11-19-2000 10:53:00 pm HvA V1.00 $
***************************************************************************/

#ifndef __CHECK_SUM_H
#define __CHECK_SUM_H

/*
 * Until C compilers support C++ namespaces, we use this
 * prefix for our namespace.
 */
#ifndef NAMESPACE
#define NAMESPACE(x)    _w32_ ## x     
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#define inchksum      NAMESPACE (inchksum)
#define inchksum_fast NAMESPACE (inchksum_fast)
#define do_checksum   NAMESPACE (do_checksum)

extern WORD inchksum    (const void *ptr, int len);
extern int  do_checksum (BYTE *buf, int proto, int len);

#ifndef cdecl
#define cdecl  /* prevent register calls */
#endif

/* chksum0.asm / chksum0.s
 */
extern WORD cdecl inchksum_fast (const void *ptr, int len); 

#if defined(USE_FAST_CKSUM)
  #define checksum(p,len) inchksum_fast(p,len)
#else
  #define checksum(p,len) inchksum(p,len)
#endif

#endif
