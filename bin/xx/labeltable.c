#include <oraccsys.h>

#undef strdup
extern char *strdup(const char *);
extern FILE *f_log;

static FILE *tab = NULL;

static void
sH(void *userData, const char *name, const char **atts)
{
  if (name[0] == 'l' && name[1] == '\0')
    {
      const char *xmlid = NULL;
      const char *label = NULL;
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i],"xml:id"))
	    xmlid = atts[i+1];
	  else if (!strcmp(atts[i],"label"))
	    label = atts[i+1];
	  if (xmlid && label)
	    break;
	}
      if (xmlid && label) /* lgs doesn't have id/label */
	fprintf(tab,"%s\t%s\n",xmlid,label);
    }
}

static void
eH(void *userData, const char *name)
{
}

#include "emain.c"

int
main(int argc, char **argv)
{
  return emain("labeltable", argc, argv, sH, eH);
}
