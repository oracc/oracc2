#include <oraccsys.h>
#include "vx.h"
#include "xnn.h"
#include "ns-xtf.h"

Hash *xtf_a = NULL, *xtf_e = NULL;

void
vx_xtf_init(void)
{
  if (xtf_a && xtf_e)
    return;
  if (!xtf_a)
    {
      xtf_a = hash_create(1024);
      struct xnn_xname *xp = xtf_anames;
      int i;
      for (i = 0; xp[i].pname; ++i)
	{
	  hash_add(xtf_a, (uccp)xp[i].pname, xp[i].qname);
	  hash_add(xtf_a, (uccp)xp[i].qname, xp[i].pname);
	}
      hash_add(xtf_a, (uccp)"xml:id", "http://www.w3.org/XML/1998/namespace:id");
      hash_add(xtf_a, (uccp)"http://www.w3.org/XML/1998/namespace:id", "xml:id");
      hash_add(xtf_a, (uccp)"xml:lang", "http://www.w3.org/XML/1998/namespace:lang");
      hash_add(xtf_a, (uccp)"http://www.w3.org/XML/1998/namespace:lang", "xml:lang");
    }
  if (!xtf_e)
    {
      xtf_e = hash_create(1024);
      struct xnn_xname *xp = xtf_enames;
      int i;
      for (i = 0; xp[i].pname; ++i)
	{
	  hash_add(xtf_e, (uccp)xp[i].pname, xp[i].qname);
	  hash_add(xtf_e, (uccp)xp[i].qname, xp[i].pname);
	}
    }
}

void
vx_xtf_term(void)
{
  if (xtf_a)
    hash_free(xtf_a, NULL);
  if (xtf_e)
    hash_free(xtf_e, NULL);
}
