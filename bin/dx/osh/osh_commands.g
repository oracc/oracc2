%{
#include <../dx.h>
#include <osh.h>
%}
struct osh_info
%%
ls, 	OSH_BUILTIN, "/usr/bin/ls", "-l"
lslr, 	OSH_BUILTIN, "/usr/bin/ls", "-lR"
env, 	OSH_BUILTIN, "/usr/bin/printenv", ""
log, 	OSH_BUILTIN, "/usr/bin/less", "$log"
build,  OSH_EXTERNAL, NULL, NULL
config, OSH_EXTERNAL, NULL, NULL
check,  OSH_EXTERNAL, NULL, NULL
init,	OSH_EXTERNAL, NULL, NULL
gloss,	OSH_EXTERNAL, NULL, NULL
list,	OSH_EXTERNAL, NULL, NULL
portal,	OSH_EXTERNAL, NULL, NULL
status,	OSH_EXTERNAL, NULL, NULL
update,	OSH_EXTERNAL, NULL, NULL
