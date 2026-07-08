#include <oraccsys.h>

int
rnv_check(const char *rnc, const char *xml)
{
  static char *rncfn, *rnvbin;

  if (!rnc)
    {
      if (!rncfn)
	{
	  fprintf(stderr, "rnv_check: called with NULL rnc");
	  return -1;
	}
    }
  if (access(rnc, R_OK))
    {
      xperror("rnv_check: %s", rnc);
      fputc('\n', stderr);
      return -1;
    }
  else
    rncfn = strdup(rnc);

  if (!xml)
    {
      if (rncfn)
	{
	  free(rncfn);
	  rncfn = NULL;
	  if (rnvbin)
	    {
	      free(rnvbin);
	      rnvbin = NULL;
	    }
	  return 0;
	}
      else
	{
	  fprintf(stderr, "rnv_check: called with NULL xml");
	  return -1;
	}
    }

  if (access(xml, R_OK))
    {
      xperror("rnv_check: %s", xml);
      fputc('\n', stderr);
      return -1;
    }

  if (!rnvbin)
    {
      char rnvbuf[strlen(oracc())+strlen("/bin/rnv0")];
      sprintf(rnvbuf, "%s/bin/rnv", oracc());
      rnvbin = strdup(rnvbuf);
    }
  
  List *args = list_create(LIST_SINGLE);
  list_add(args, rnvbin);
  list_add(args, " ");
  list_add(args, (void*)rncfn);
  list_add(args, " ");
  list_add(args, (void*)xml);

  unsigned char *syscmd = list_concat(args);
  int sys = system((ccp)syscmd);
  free(syscmd);
  
  if (sys)
    {
      int xstatus = WEXITSTATUS(sys);
      if (xstatus > 1)
	return xstatus;
      else
        {
          fprintf(stderr,
		  "rnv_check: failed system call: %s %s %s\n",
		  rnvbin, rnc, xml);
          return -1;
        }
      
    }
  else
    return 0;
}
