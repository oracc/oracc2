#ifndef DX_H_
#define DX_H_

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>

#define DX_SERVER_NAME "/tmp/oracc-daemon-dx"

extern const char *progname;

extern char *dx_session(char **sesh_id);
extern int dx_accept(int listenfd, uid_t * uidptr);
extern int dx_connect (const char *name);
extern int dx_exec(char **keys, int nkeys, char **err, char *tmpdir);
extern int dx_listen (const char *name);
extern void dx_prechecks(void);
extern void dx_process(int sock, char *buf);
extern void dx_signal(void);

#endif/*DX_H_*/
