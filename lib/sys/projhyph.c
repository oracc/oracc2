#include <oraccsys.h>

char *
projhyph(const char *p)
{
  static char *buf = NULL;
  static int buf_len = 0;

  if (!p)
    {
      free(buf);
      buf = NULL;
      buf_len = 0;
      return NULL;
    }

  int xbuf_len = buf_len;
  while (buf_len < (strlen(p)+1))
    buf_len += 1024;
  if (xbuf_len != buf_len)
    buf = realloc(buf, buf_len);

  strcpy(buf, p);
  char *h = buf;
  while ((h = strchr(h,'/')))
    *h++ = '-';

  return buf;
}
