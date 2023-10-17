#ifndef ATF2UTF_H_
#define ATF2UTF_H_
#include <stddef.h>
#include <sys/types.h>

#include <mesg.h>
#include <unidef.h>

extern unsigned char *atf2utf(Mloc *locp, register const unsigned char *s, int rx);
extern void atf2utf_init(void);
extern unsigned char *Unicode2UTF (unsigned short ch);

#endif /*ATF2UTF_H_*/
