#include <oraccsys.h>
#include "px.h"

static char *px_error = NULL;

unsigned char **
px_loadfile_lines3(unsigned const char *fname, size_t *nlines, unsigned char **fmem)
{
  size_t i, j, n, l;
  unsigned char *f = NULL;
  unsigned char **lp = NULL;

  if ('-' == *fname && !fname[1])
    f = loadstdin(&n);
  else
    f = px_loadfile(fname,&n);

  if (f)
    {
      *fmem = f;
      
      for (i = l = 0; i < n; ++i)
	if ('\n' == f[i])
	  ++l;
      lp = malloc((l+1) * sizeof(unsigned char *));
      for (i = j = 0; i < n; ++i)
	{
	  lp[j++] = &f[i];
	  while (i < n && f[i] != '\n')
	    ++i;
	  f[i] = '\0';
	}
      lp[j] = NULL;
      if (nlines)
	*nlines = l;
    }
  return lp;
}

unsigned char *
px_loadfile_error(void)
{
  unsigned char *tmp = (unsigned char *)px_error;
  px_error = NULL;
  return tmp;
}

unsigned char *
px_loadfile(unsigned const char *fname, size_t *nbytes)
{
  struct stat finfo;
  unsigned char *ftext;
  size_t fsize;
  int fdesc;

  if (fname == NULL)
    {
      px_error = strdup("loadfile: must give filename argument");
      goto error;
    }
  if (-1 == stat((const char*)fname,&finfo))
    {
      int n = snprintf(NULL,0,"loadfile: stat failed on %s",fname);
      px_error = malloc(n+1);
      sprintf(px_error,"loadfile: stat failed on %s",fname);
      goto error;
    }
  if (!S_ISREG(finfo.st_mode))
    {
      int n = snprintf(NULL,0,"loadfile: %s not a regular file\n",fname);
      px_error = malloc(n+1);
      sprintf(px_error,"loadfile: %s not a regular file\n",fname);
      goto error;
    }
  fsize = finfo.st_size;
  if (NULL == (ftext = malloc(fsize+1)))
    {
      int n = snprintf(NULL,0,"loadfile: %s: couldn't malloc %ld bytes\n",
		       fname,(unsigned long)fsize);
      px_error = malloc(n+1);
      sprintf(px_error,"loadfile: %s: couldn't malloc %ld bytes\n",
	      fname,(unsigned long)fsize);
      goto error;
    }
  fdesc = open((const char*)fname,O_RDONLY);
  if (fdesc >= 0)
    {
      ssize_t ret = read(fdesc,ftext,fsize);
      close(fdesc);
      if (ret != fsize)
	{
	  int n = snprintf(NULL,0,"loadfile: %s: read %ld bytes failed\n", fname, (unsigned long)fsize);
	  px_error = malloc(n+1);
	  sprintf(px_error,"loadfile: %s: read %ld bytes failed\n", fname, (unsigned long)fsize);
	  goto error;
	}
      if (nbytes)
	*nbytes = ret;
      ftext[ret] = '\0';
    }
  else
    {
      int n = snprintf(NULL,0,"loadfile: %s: open failed (system error %d: %s)\n", fname, errno, strerror(errno));
      px_error = malloc(n+1);
      sprintf(px_error, "loadfile: %s: open failed (system error %d: %s)\n", fname, errno, strerror(errno));
      goto error;
    }

  return ftext;

 error:
  return NULL;
}
