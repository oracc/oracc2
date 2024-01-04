#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <runexpat.h>
#include <tis.h>

Tisp tish;

const char **
tis_attrs(const char *xid, const char *name, const char **atts, FILE *fp)
{
  const char **natts = NULL;
  struct tis_data *tdata = hash_find(tish, (ucp)xid);
  if (tdata)
    {
      int i;
      for (i = 0; atts[i]; ++i)
	;
      natts = malloc((i+9)*sizeof(char *));
      memcpy(natts, atts, i*sizeof(char*));
      natts[i++] = "xmlns:is";
      natts[i++] = "http://oracc.org/ns/is/1.0";
      natts[i++] = "is:cnt";
      natts[i++] = tdata->cnt;
      natts[i++] = "is:pct";
      natts[i++] = tdata->pct;
      natts[i++] = "is:ref";
      natts[i++] = tdata->ref;
      natts[i] = NULL;
      return natts;
    }
  else
    return atts;
}

void
tis_island(const char *xid, const char *name, const char **atts, FILE *fp)
{
  struct tis_data *tdata = hash_find(tish, (ucp)xid);
  if (tdata)
    {
      if (tdata->subs)
	{
	  struct tis_data *sdata;
	  fputs("<is:land>", fp);
	  for (sdata = list_first(tdata->subs); sdata; sdata = list_next(tdata->subs))
	    {
	      char *k = strdup(strchr(sdata->key,'|')+1), *c, *v;
	      c = strchr(k, '=');
	      *c++ = '\0';
	      while ('0' == *c)
		++c;
	      v = strchr(c, '/');
	      *v++ = '\0';
	      fprintf(fp, "<is:data k=\"%s\" v=\"%s\" c=\"%s\" cnt=\"%s\" pct=\"%s\" ref=\"%s\"/>",
		      k, v, c, sdata->cnt, sdata->pct, sdata->ref);
	    }
	  fputs("</is:land>", fp);
	}
    }
}

int
main(int argc, char **argv)
{
  FILE *tfp = fopen(argv[1], "r");
  tis_load_init();
  tish = tis_load(tfp);
  xmlfilter_actions(tis_attrs, NULL, tis_island);
  xmlfilter(NULL, stdout);
  tis_load_term(tish);
}
