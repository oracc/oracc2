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
      return " cfy-j-spread";
    case ELT_Jp:
      return " cfy-j-penult";
    case ELT_Jcs:
      return " cfy-j-cspread";
    case ELT_Jcp:
      return " cfy-j-cpenult";
    default:
      return "";
    }
}
