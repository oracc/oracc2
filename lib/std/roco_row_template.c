#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <xmlify.h>
#include <roco.h>
#include <vector.h>

#define ccp const char *
#define uccp unsigned const char *
extern int verbose;

/* Create a roco_row_format template which will output Roco r
 * according to the field sequence in template t.  If parameter
 * add_empty==1, insert empty columns for missing fields; otherwise
 * silently drop them
 */
char *
roco_row_template(Roco *r, char **v, int add_empty)
{
  if (!r || !r->fields_from_row1 || !v)
    return NULL;

  if (!r->fields)
    roco_field_indexes(r);
  
  int i;

  if (verbose)
    {
      fputs("v[]=", stderr);
      for (i = 0; v[i]; ++i)
	fprintf(stderr, "\t%s", v[i]);
      fputc('\n', stderr);
    }

  List *tlp = list_create(LIST_SINGLE);
  for (i = 0; v[i]; ++v)
    {
      int f = (uintptr_t)hash_find(r->fields, (uccp)v[i]);
      if (f)
	list_add(tlp, (void*)(intptr_t)f);
      else if (add_empty)
	list_add(tlp, (void*)(intptr_t)-1);
    }

  char buf[list_len(tlp)*5];
  *buf = '\0';
  void *vp;
  for (vp = list_first(tlp); vp; vp = list_next(tlp))
    {
      if ((intptr_t)vp > 0)
	sprintf(buf+strlen(buf), "%%%lu", (intptr_t)vp);
      strcat(buf, "\t");
    }
  buf[strlen(buf)-1] = '\0';
  return strdup(buf);
}
