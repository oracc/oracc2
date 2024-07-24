#include <oraccsys.h>
#include <sys/mman.h>
#include "iss.h"

#define MAX_PATH 1024

extern const char *prog;
extern int verbose;
extern struct sortinfo *sip;

struct sortinfo *
si_load_csi(Isp *ip)
{
  struct stat csistat;
  int csifd;
  void *csimap;
  char csiname[MAX_PATH];

  if (ip->form && !strcmp(ip->form, "sources"))
    sprintf(csiname,"%s/pub/%s/sortinfo.csi",oracc(),ip->itemdata.proj);
  else
    sprintf(csiname,"%s/pub/%s/sortinfo.csi",oracc(),ip->project);
  ip->cache.csi = (char*)pool_copy((ucp)csiname, ip->p);

  if (stat(csiname,&csistat))
    {
      if (ip)
	ip->err = "stat failed on sortinfo.csi";
      fprintf(stderr,"%s: %s: stat failed\n",prog,csiname);
      return NULL;
    }
  if ((csifd = open((const char *)csiname,O_RDONLY)) < 0)
    {
      if (ip)
	ip->err = "open failed on sortinfo.csi";
      fprintf(stderr,"%s: %s: open failed\n",prog,csiname);
      return NULL;
    }
  if (csistat.st_size < sizeof(struct sortinfo))
    {
      if (ip)
	ip->err = "sortinfo.csi too small to be valid";
      fprintf(stderr,"%s: %s: .csi file too small to be valid\n",prog,csiname);
      return NULL;
    }
  if (MAP_FAILED == (csimap = mmap(0, csistat.st_size, 
				   PROT_READ, MAP_SHARED, csifd, 0)))
    {
      if (ip)
	ip->err = "mmap sortinfo.csi failed";
      fprintf(stderr,"%s: %s: mmap failed\n",prog,csiname);
      return NULL;
    }
  sip = malloc(sizeof(struct sortinfo));
  *sip = *(struct sortinfo*)csimap;
  sip->fields = (struct si_field*)((char*)csimap + (uintptr_t)sip->fields);
  sip->idlist = (u4*)((char*)csimap + (uintptr_t)sip->idlist);
  sip->scodes = (s4*)((char*)csimap + (uintptr_t)sip->scodes);
  sip->pindex = (u4*)((char*)csimap + (uintptr_t)sip->pindex);
  sip->pool = (unsigned char *)((char*)csimap + (csistat.st_size - sip->pool_len));
  return sip;
}
