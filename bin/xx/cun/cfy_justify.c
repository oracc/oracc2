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

char *
cfy_justify_charspace(const char *u)
{
  size_t ulen;
  wchar_t *w = utf2wcs((uccp)u, &ulen);
  wchar_t nw[2*ulen+1];
  int i, j;
  for (i = j = 0; i < ulen; ++i)
    {
      if (w[i+1] == 0x200d)
	{
	  nw[j++] = w[i++];
	  nw[j++] = w[i++];
	  nw[j++] = w[i];
	}
      else
	{
	  nw[j++] = w[i];
	  nw[j++] = ' ';
	}
    }
  nw[j] = '\0';
  free(w);
  return (char *)wcs2utf(nw, j);
}
