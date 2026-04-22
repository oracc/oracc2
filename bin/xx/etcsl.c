#include <oraccsys.h>
#include <runexpat.h>

/* A throwaway program to extract a table of ETCSL IDs to line labels
   for use in aligning the ETCSL translations with the Oracc
   versions */

extern FILE *f_log;

static FILE *tab = NULL;

int etcsl_pending = 0;

#define BLEN 31
char curr_q[BLEN+1] = { '\0' } ;
char curr_etcsl[BLEN+1];
char curr_xid[BLEN+1];
char curr_lab[BLEN+1];

static void save(const char *data, char *buf);

static void
etcsl(const char *e)
{
  if ('t' == *e)
    save(strchr(e,'=')+1, curr_etcsl);
  else
    fprintf(tab,"%s\t%s\t%s\t%s\t%s\n", curr_q, curr_etcsl, curr_xid, curr_lab, strchr(e,'=')+1);
  etcsl_pending = 0;
}

static void
save(const char *data, char *buf)
{
  if (strlen(data)>BLEN)
    fprintf(stderr, "etcsl: data len for %s = %d; increase BLEN\n", data, (int)strlen(data));
  else
    strcpy(buf, data);    
}

static void
sH(void *userData, const char *name, const char **atts)
{
  if (name[0] == 'l' && name[1] == '\0')
    {
      const char *type = findAttr(atts, "type");
      if (!type || !*type) /* we don't want data from anything but untyped, i.e., mts, lines */
	{
	  int i;
	  for (i = 0; atts[i] != NULL; i+=2)
	    {
	      if (!strcmp(atts[i],"xml:id"))
		{
		  save(atts[i+1], curr_xid);
		  if (!*curr_q)
		    {
		      strcpy(curr_q, curr_xid);
		      char *dot = strchr(curr_q, '.');
		      *dot = '\0';
		    }
		}
	      else if (!strcmp(atts[i],"label"))
		save(atts[i+1], curr_lab);
	    }
	}
    }
  else if (!strcmp(name, "protocol"))
    {      
      const char *type = findAttr(atts, "type");
      if (!strcmp(type, "etcsl"))
	etcsl_pending = 1;
    }
}

static void
eH(void *userData, const char *name)
{
   /* This is always a closing </protocol> when etcsl_pending != ETCSL_NOPE */
  if (etcsl_pending)
    etcsl(charData_retrieve());
  else
    {
      if (!strcmp(name, "composite"))
	*curr_q = '\0';
      (void)charData_retrieve();
    }
}

#include "emain.c"

int
main(int argc, char **argv)
{
  tab = stdout;
  return emain("labeltable", argc, argv, sH, eH);
}
