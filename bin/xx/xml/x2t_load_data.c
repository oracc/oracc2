#include <oraccsys.h>
#include <runexpat.h>
#include "xml2tsv.h"

static int depth;
static int rown = 0, coln = 0;
static Pool *hp, *p;

void
load_fmp_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "ROW"))
    {
      coln = 0;
      ++rown;
    }
}

void
load_fmp_eH(void *userData, const char *name)
{
  if (!strcmp(name, "COL"))
    {
      r->rows[rown][coln] = hpool_copy((uccp)charData_retrieve(), hp);
      char *t = (char*)r->rows[rown][coln];
      while (*t)
	{
	  if ('\t' == *t || '\r' == *t || '\n' == *t)
	    *t++ = ' ';
	  else
	    ++t;
	}
      ++coln;
    }
  else if (!strcmp(name, "METADATA"))
    (void)charData_retrieve();
}

void
load_xml_sH(void *userData, const char *name, const char **atts)
{
  if (depth == 1)
    {
      ++rown;
      const char *xid = get_xml_id(atts);
      if (*xid)
	{
	  coln = (int)(uintptr_t)hash_find(r->fields, (uccp)"id_attr");
	  r->rows[rown][coln-1] = pool_copy((uccp)xid, p);
	}
    }
  ++depth;
}

void
load_xml_eH(void *userData, const char *name)
{
  --depth;
  if (depth == 2)
    {
      coln = (int)(uintptr_t)hash_find(r->fields, (uccp)name);
      if (coln)
	{
	  r->rows[rown][coln-1] = hpool_copy((uccp)charData_retrieve(), hp);
	  char *t =  (char*)r->rows[rown][coln-1];
	  while ((t = strchr(t, '\t')))
	    {
	      fprintf(stderr, "untab\n");
	      *t++ = ' ';
	    }
	}
      else
	fprintf(stderr, "xml2tsv: unknown field %s at depth==2\n", name);
    }
}

void
x2t_load_data(const char *fn, Roco *r)
{
  depth = 0;
  runexpat_omit_rp_wrap();
  char const *fnlist[2];
  fnlist[0] = fn;
  fnlist[1] = NULL;
  p = pool_init();
  hp = hpool_init();
  runexpat(i_list, fnlist,
	   fmp_mode ? load_fmp_sH : load_xml_sH,
	   fmp_mode ? load_fmp_eH : load_xml_eH);
}
