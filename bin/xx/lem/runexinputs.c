#include <oraccsys.h>
static Pool *reip = NULL;

const uchar **
runexinputs(const char *f)
{
  if (!reip)
    reip = pool_init();
  List *files = list_create(LIST_SINGLE);
  if (f)
    {
      list_add(files,pool_copy((ucp)f, reip));
    }
  else
    {
      char fnbuf[_MAX_PATH], *fn;
      while ((fn = fgets(fnbuf,_MAX_PATH,stdin)))
	{
	  fn[strlen(fn)-1] = '\0';
	  list_add(files,pool_copy((ucp)fn, reip));
	}
    }
  const uchar **fnlist = malloc((1+list_len(files)) * sizeof(const char *));
  int i = 0;
  for (f = list_first(files); f; f = list_next(files))
    fnlist[i++] = pool_copy((ucp)expand(NULL,f,"xtf"), reip);
  fnlist[i] = NULL;
  list_free(files, NULL);
  return fnlist;
}
