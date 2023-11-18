#ifndef DX_H_
#define DX_H_

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>

#define DX_SOCK_NAME "/tmp/oracc-daemon-dx"

extern void dx_process(int sock, char *buf);
extern char *dx_session(char **sesh_id);
extern int dx_exec(char **keys, int nkeys, char **err, char *tmpdir);

#endif/*DX_H_*/
