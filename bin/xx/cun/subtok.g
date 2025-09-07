%{
#include <oraccsys.h>
#include <stddef.h>
#include "cfy.h"
%}
struct subtok;
%%
"data", offsetof(Elt,data), V_STR
"etype", offsetof(Elt,etype), V_INT
"ELT_G", ELT_G, V_INT
