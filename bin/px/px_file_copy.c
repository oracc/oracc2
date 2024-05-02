#include <oraccsys.h>
#include <px.h>

int
px_file_copy(Isp *ip, const char *fn, const char *to)
{
  if (access(fn, R_OK))
    {
      printf("<span class=\"error\"> "
	     PX_ERROR_START
	     " file_copy: file %s non-existent or unreadable</span>\n", fn);
      return 1;
    }
  file_copy(fn, to);
}
