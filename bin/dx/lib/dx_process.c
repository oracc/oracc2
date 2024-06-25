#include <dx.h>
#include <string.h>

static char **buf2keys(char *buf, int *n);
static int dx_status_file(const char *tmpdir);

void
dx_process(int sock, char *buf)
{
  char sesh_buf[1024];
  char *sesh_id = NULL;
  char *tmp = dx_session(&sesh_id);

  if (dx_status_file(tmp))
    {
      write(sock, "error", 5);
      return;
    }

  sprintf(sesh_buf, "%s::%s\n", tmp, sesh_id);
  if (write(sock, sesh_buf, strlen(sesh_buf)) < 0)
    perror("writing on stream socket");
  fprintf(stderr, "%s: closing socket [%d]\n", progname, sock);
  close(sock);
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
  fprintf(stderr, "buf2keys:");
  for (i = 0; k[i]; ++i)
    fprintf(stderr, " %s", k[i]);
  fprintf(stderr, "\n");
  return k;
}

static int
dx_status_file(const char *tmpdir)
{
  int statusfd;
  char buf[strlen(tmpdir)+strlen("/statusX")];
  sprintf(buf,"%s/status", tmpdir);
  if ((statusfd = open(buf, O_WRONLY|O_CREAT, 0664)) < 0)
    perror("creating status file");
  else
    {
      if (3 != write(statusfd, "run", 3))
	perror("writing 'run' to status file");
      else
	{
	  if (close(statusfd))
	    perror("closing status file");
	  else
	    return 0;
	}
    }
  return 1;
}
