#include <dx.h>

static char **buf2keys(char *buf, int *n);

void
dx_process(int sock, char *buf)
{
  char sesh_buf[1024];
  char *sesh_id = NULL;
  char *tmp = dx_session(&sesh_id);
  sprintf(sesh_buf, "%s::%s\n", tmp, sesh_id);
  if (write(sock, sesh_buf, strlen(sesh_buf)) < 0)
    perror("writing on stream socket");
  fprintf(stderr, "%s: closing socket [%d]\n", progname, sock);
  char **keys = NULL;
  int nkeys = 0;
  fprintf(stderr, "%s: making keys from '%s'\n", progname, buf);
  keys = buf2keys(buf, &nkeys);
  char *err = NULL;
  if (dx_exec(keys, nkeys, &err, tmp))
    perror(err);
}

static char **
buf2keys(char *buf, int *n)
{
  int nk = 1, i = 0;
  char *s = buf, **k;
  while (*s)
    {
      if (isspace(*s))
	{
	  ++nk;
	  while (isspace(*s))
	    ++s;
	}
      else
	++s;
    }
  k = malloc((1+nk)*sizeof(char*)); /* This is a memory leak */
  s = buf;
  k[i++] = s;
  while (*s)
    {
      if (isspace(*s))
	{
	  *s++ = '\0';
	  while (isspace(*s))
	    ++s;
	  if (*s)
	    k[i++] = s++;
	}
      else
	++s;
    }
  k[i] = NULL;
  *n = nk;
  fprintf(stderr, "buf2keys: ");
  for (i = 0; k[i]; ++i)
    fprintf(stderr, " %s", k[i]);
  fprintf(stderr, "\n");
  return k;
}
