#include <oraccsys.h>
#include "cfy.h"

const char *
cfy_justify_class(Etype j)
{
  switch (j)
    {
    case ELT_Jl:
      return " cfy-j-left";
    case ELT_Jr:
      return " cfy-j-right";
    case ELT_Jc:
      return " cfy-j-centre";
    case ELT_Js:
    case ELT_Jp:
    case ELT_Jcs:
    case ELT_Jcp:
    default:
      return " cfy-j-spread";
    }
}
