#include <oraccsys.h>
int
xmkdirs(const char *dir)
{
  struct stat sb;
  int err = 0;
  if (stat(dir, &sb) || !S_ISDIR(sb.st_mode))
    {
      if (strchr(dir, '/'))
	{
	  char xdir[strlen(dir)+1];
	  strcpy(xdir, dir);
	  if ('/' == xdir[strlen(xdir)-1])
	    xdir[strlen(xdir)-1] = '\0';
	  char *slash = xdir+1;
	  while ((slash = strchr(slash,'/')))
	    {
	      *slash = '\0';
	      if (stat(xdir, &sb) || !S_ISDIR(sb.st_mode))
		{
		  if ((err = xmkdir(xdir, 0775, 0)))
		    break;
		}
	      *slash++ = '/';
	    }
	  if (!err)
	    err = xmkdir(xdir, 0775, 0);
	}
      else
	err = xmkdir(dir, 0775, 0);
    }

  return err;
}
