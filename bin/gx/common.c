#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype128.h>
#include <pool.h>
#include <cbdyacc.h>
#include <cbd.h>
#include "gx.h"

static Pool *common_pool = NULL;

void
common_init(void)
{
  common_pool = pool_init();
}

void
common_term(void)
{
  pool_term(common_pool);
  common_pool = NULL;
}

unsigned char *
check_bom(unsigned char *s)
{
  if (s[0] == 0xef && s[1] == 0xbb && s[2] == 0xbf)
    return s+3;
  else if ((s[0] == 0x00 && s[1] == 0x00 && s[2] == 0xfe && s[3] == 0xff)
	   || (s[0] == 0xff && s[1] == 0xfe && s[2] == 0x00 && s[3] == 0x00)
	   || (s[0] == 0xfe && s[1] == 0xff)
	   || (s[0] == 0xff && s[1] == 0xfe))
    {
      fprintf(stderr,"unhandled UTF-format (I only understand UTF-8)\n");
      return NULL;
    }
  else
    return s;
}

unsigned char **
setup_lines(unsigned char *ftext)
{
  unsigned char **p,**ret;
  register unsigned char*s = ftext;
  int nlines = 0;
  while (*s)
    {
      if (*s == '\r')
        {
          ++nlines;
          if (s[1] != '\n')
            *s++ = '\n';     /* for MAC \r, map to \n */
          else
            s+=2;            /* for DOS \r\n, skip \n */
        }
      else if (*s == '\n') /* UNIX */
        {
          ++nlines;
          ++s;
        }
      else
        {
          ++s;
        }
    }
  if (s[-1] != '\n' && s[-1] != '\r')
    ++nlines;
  ++nlines; /* NULL ptr to terminate */
  ret = p = malloc(nlines*sizeof(unsigned char *));
  s = ftext;
  while (*s)
    {
      *p++ = s;
      while (*s && '\n' != *s)
        ++s;
      if (*s == '\n')
        {
          if (s > ftext && s[-1] == '\r')
            {
              s[-1] = '\0';
              ++s;
            }
          else
            *s++ = '\0';
        }
    }
  *p = NULL;
  return ret;
}

unsigned char *
slurp(const char *caller, const char *fname, ssize_t *fsizep)
{
  struct stat finfo;
  extern const char *file;
  int fdesc;
  ssize_t fsize;
  unsigned char *ftext = NULL;
  if (-1 == stat(fname,&finfo))
    {
      fprintf(stderr,"%s: stat failed on %s\n",caller,fname);
      return NULL;
    }
  if (!S_ISREG(finfo.st_mode))
    {
      fprintf(stderr,"%s: %s not a regular file\n",caller,fname);
      return NULL;
    }
  fsize = finfo.st_size;
  if (!fsize)
    {
      fprintf(stderr,"%s: %s: empty file\n",caller,fname);
      return NULL;
    }
  if (NULL == (ftext = malloc(fsize+1)))
    {
      fprintf(stderr,"%s: %s: couldn't malloc %d bytes\n",
              caller,fname,(int)fsize);
      return NULL;
    }

  fdesc = open(fname,O_RDONLY);
  if (fdesc >= 0)
    {
      ssize_t ret = read(fdesc,ftext,fsize);
      close(fdesc);
      if (ret == fsize)
        ftext[fsize] = '\0';
      else
        {
          fprintf(stderr,"%s: read %d bytes failed\n",caller,(int)fsize);
	  free(ftext);
          return NULL;
        }
    }
  else
    {
      fprintf(stderr, "%s: %s: open failed\n",caller,fname);
      free(ftext);
      return NULL;
    }

  file = errmsg_fn ? errmsg_fn : fname;
  if (fsizep)
    *fsizep = fsize;
  return ftext;
}

unsigned char *
tok(unsigned char *s, unsigned char *end)
{
  while (isspace(*s))
    ++s;
  if (*s)
    {
      unsigned char *e = s;
      while (!isspace(*e))
	++e;
      *end = *e;
      if (*e)
	*end = '\0';
    }
  else
    *end = '\0';     
  return s;
}

void
untab(unsigned char *s)
{
  while (*s)
    if ('\t' == *s)
      *s++ = ' ';
    else
      ++s;
}

