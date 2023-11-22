%{
#include <../dx.h>
#include <osh.h>
%}
struct osh_info
%%
ls, 	OSH_BUILTIN, "/usr/bin/ls", "-l"
lslr, 	OSH_BUILTIN, "/usr/bin/ls", "-lR"
env, 	OSH_BUILTIN, "/usr/bin/printenv", ""
config, OSH_EXTERNAL, NULL, NULL
check,  OSH_EXTERNAL, NULL, NULL
init,	OSH_EXTERNAL, NULL, NULL
list,	OSH_EXTERNAL, NULL, NULL
status,	OSH_EXTERNAL, NULL, NULL
