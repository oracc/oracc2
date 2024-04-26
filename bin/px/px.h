#ifndef PX_H_
#define PX_H_

#include "pxdefs.h"

#include <stdlib.h>
#include "isp/isp.h"
#include "pui/pui.h"

extern struct ispargstab *pxargs (register const char *str, register size_t len);
extern int px_validate(Isp *ip);
extern int px_options(int argc, char **argv, Isp *ip);

#endif/*PX_H*/
