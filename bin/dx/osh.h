#ifndef OSH_H_
#define OSH_H_
enum osh_cmd { OSH_BUILTIN , OSH_EXTERNAL };
struct osh_info
{
  const char * name;
  enum osh_cmd type;
  const char *cmd;
  const char *arg;
};

extern const char *project;
extern const char *user;
extern int ssh_mode;
extern int verbose;

extern char **osh_options(int argc, char **argv);
extern void osh_exec(const char *cmd, const char *arg);
extern void osh_dcx(char **optv);
extern void osh_project(void);
extern struct osh_info *osh_commands (register const char *str, register size_t len);

#endif/*OSH_H*/
