#include <oraccsys.h>
#include "../px.h"
#include "what.h"

#define GENERATOR "px frag "

int
what_file(Isp *ip)
{
  char xfragx[_MAX_PATH];
  sprintf(xfragx, "%s/bin/xfragx", oracc());
  wpx_print_hdr(ip);
  fprintf(stderr, "%s -hs4 -p %s -g %s %s %s\n",
	  xfragx, ip->project, GENERATOR, ip->itemdata.file, ip->itemdata.block);
  execl(xfragx, "xfragx", 
	"-hs4",
	"-p", ip->project,
	"-g", GENERATOR,
	ip->itemdata.file, ip->itemdata.block, NULL);
  ip->err = "fatal: xfragx call failed";
  return PX_ERROR;
}
