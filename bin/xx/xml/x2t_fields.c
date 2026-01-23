#include <oraccsys.h>
#include <runexpat.h>
#include "xml2tsv.h"

static int depth;
static Hash *hfields;
static List *lfields;

void
fmp_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "FIELD"))
    {
      const char *name = findAttr(atts, "NAME");
      list_add(lfields, strdup(name));
    }
  else if (!strcmp(name, "ROW"))
    ++nrec;
}

void
xml_sH(void *userData, const char *name, const char **atts)
{
  static int tried_xid = 0;
  if (depth == 1)
    {
      ++nrec;
      if (!tried_xid)
	{
	  const char *xid = get_xml_id(atts);
	  if (*xid)
	    hash_add(hfields, (uccp)"id_attr", "");
	  tried_xid = 1;
	}
    }
  else if (depth == 2)
    {
      if (!hash_find(hfields, (uccp)name))
	hash_add(hfields, (uccp)strdup(name), "");
    }
  else if (depth > 2)
    {
      fprintf(stderr, "x2t_fields: field %s has structured content\n", name);
    }
  ++depth;
}

void
fmp_eH(void *userData, const char *name)
{
}

void
xml_eH(void *userData, const char *name)
{
  --depth;
}

void
x2t_set_fields(const char *fn)
{
  depth = 0;
  runexpat_omit_rp_wrap();
  char const *fnlist[2];
  fnlist[0] = fn;
  fnlist[1] = NULL;
  if (fmp_mode)
    lfields = list_create(LIST_SINGLE);
  else
    hfields = hash_create(128);
  runexpat(i_list, fnlist, fmp_mode ? fmp_sH : xml_sH, fmp_mode ? fmp_eH : xml_eH);
  if (fmp_mode)
    fields = (const char**)list2array_c(lfields, &nfld);
  else
    fields = hash_keys2(hfields, &nfld);
  if (verbose)
    {
      int i;
      fprintf(stderr, "x2t_fields: %ld records with %d fields named ", nrec, nfld);
      for (i = 0; fields[i]; ++i)
	fprintf(stderr, "%s%s", fields[i], fields[i+1] ? "," : "\n");
    }
  hash_free(hfields, NULL);
}
