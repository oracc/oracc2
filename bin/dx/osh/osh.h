#ifndef OSH_H_
#define OSH_H_

#include <sys/types.h>

enum osh_cmd { OSH_BUILTIN , OSH_EXTERNAL };
struct osh_info
{
  const char * name;
  enum osh_cmd type;
  const char *cmd;
  const char *arg;
};

typedef struct job
{
  char **cmdv;
  char *cmd;
  char *cmd1;
  char *user;
  char *project;
  char *sesh;  /* session component of tmpdir */
  char *log;   /* tmpdir/request.log */
  char *tmp;   /* tmpdir */
  char *statusfile;/* tmpdir/status */
  char *phome; /* The oracc/home directory for the project */
  char *pwork; /* The working directory, which may include the subproject */
  pid_t pid;
  char status[6];    /* run done error */
  int time;
} Job;

extern const char *progname;
extern int ssh_mode;
extern int verbose;

extern char **osh_options(int argc, char **argv, Job *jp);
extern int osh_dcx(char **optv, Job *jp);
extern int osh_exec(const char *cmd, const char *arg, Job *jp);
extern int osh_project(char **optv, Job *jp);
extern int osh_wait(Job *jp);
extern int osh_wrapup(Job *jp);
extern struct osh_info *osh_commands (register const char *str, register size_t len);
extern void osh_cleanup(Job *jp);

#endif/*OSH_H*/
