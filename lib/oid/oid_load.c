#include <stdlib.h>
#include <stdio.h>
#include <oraccsys.h>
#include <loadfile.h>
#include <hash.h>
#include <oid.h>

static const char *oidtab = NULL;

void
oid_set_oidtab(const char *s)
{
  oidtab = s;
}

static char *
oid_tab_path(const char *base)
{
  int len = strlen(oracc_home()) + strlen("/oid/.tab") + strlen(base) + 1;
  char *tmp = malloc(len);
  sprintf(tmp, "%s/oid/%s.tab", oracc_home(), base);
  return tmp;
}

char *
oid_tab(void)
{
  if (oidtab)
    {
      if (!strcmp(oidtab, "oid"))
	return oid_tab_path("oid");
      else if (!strcmp(oidtab, "xid"))
	return oid_tab_path("xid");
      else
	return strdup(oidtab);
    }
  else
    return oid_tab_path("oid");
}

Oids *
oid_load(void)
{
  Oids *o = calloc(1, sizeof(Oids));
  o->lines = loadfile_lines3((uccp)(o->file = oid_tab()), (size_t*)&o->nlines, &o->mem);
  qsort(o->lines, o->nlines, sizeof(unsigned char *), cmpstringp);
  return o;
}

Oids *
oid_load_keys(const char *file)
{
  Oids *o = calloc(1, sizeof(Oids));
  o->file = file;
  o->lines = loadfile_lines3((uccp)o->file, (size_t*)&o->nlines, &o->mem);
  return o;
}

Hash *
oid_load_sort(const char *domain)
{
  char *p = oid_tab_path("oid");
  char *b = strrchr(p, '/');
  b[1] = '\0';
  char d[strlen(p)+strlen("oid- -sort.txt")+strlen(domain)];
  sprintf(d, "%soid-%s-sort.txt",p,domain);
  unsigned char **ll = loadfile_lines((uccp)d, NULL);
  Hash *s = hash_create(4096);
  if (ll)
    {
      int i;
      for (i = 0; ll[i]; ++i)
	hash_add(s, ll[i], (void*)(uintptr_t)i);
    }
  return s;
}
