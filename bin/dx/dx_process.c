#include <dx.h>

void
dx_process(int sock, char *buf)
{
  char sesh_buf[1024];
  char *sesh_id = NULL;
  char *tmp = dx_session(&sesh_id);
  sprintf(sesh_buf, "%s::%s\n", tmp, sesh_id);
  if (write(sock, sesh_buf, strlen(sesh_buf)) < 0)
    perror("writing on stream socket");
  printf("closing dcx socket %d\n", sock);
  char *keys[] = { "build", NULL };
  int nkeys = 1;
  char *err = NULL;
  if (dx_exec(keys, nkeys, &err, tmp))
    perror(err);
}
