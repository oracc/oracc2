#include <oraccsys.h>
#include "cx.h"

int
cx_uniq(const char *fn)
{
  if (!fn)
    fn = "-";

  const char **lp = (const char **)loadfile_lines3((uccp)fn, NULL, NULL);
  if (lp && lp[0] && !strncmp(lp[0], "id_", 3))
    {
      int i;
      Hash *seen = hash_create(20000);
      Pool *p = pool_init();
      for (i = 0; lp[i]; ++i)
	{
	  char id[8];
	  strncpy(id, lp[i], 7);
	  id[7] = '\0';
	  if (!hash_find(seen, (uccp)id))
	    {
	      hash_add(seen, pool_copy((uccp)id, p), "");
	      puts(lp[i]);
	    }
	  else
	    {
	      fprintf(stderr, "cx: uniq-mode: dropping %s\n", id);
	    }
	}
      fclose(stdout);
      return 0;
    }
  else
    fprintf(stderr, "cx: catalogue %s is empty or does not start with 'id_'\n", fn);
  return 1;
}
