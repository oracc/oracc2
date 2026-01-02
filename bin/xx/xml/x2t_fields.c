#include <oraccsys.h>
#include <runexpat.h>
#include "xml2tsv.h"

static int depth;
static Hash *hfields;

void
fmp_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "FIELD"))
    {
      const char *name = findAttr(atts, "NAME");
      hash_add(hfields, (uccp)strdup(name), "");
    }
  else if (!strcmp(name, "ROW"))
    ++nrec;
}

void
xml_sH(void *userData, const char *name, const char **atts)
{
  if (depth == 1)
    ++nrec;
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
  hfields = hash_create(128);
  runexpat(i_list, fnlist, fmp_mode ? fmp_sH : xml_sH, fmp_mode ? fmp_eH : xml_eH);
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
