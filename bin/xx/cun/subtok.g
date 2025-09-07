%{
#include <oraccsys.h>
#include <stddef.h>
#include "cfy.h"
%}
struct subtok;
%%
"data", offsetof(Elt,data), T_OFF, V_STR
"etype", offsetof(Elt,etype), T_OFF, V_INT
"ELT_G", ELT_G, T_ELT, V_INT
