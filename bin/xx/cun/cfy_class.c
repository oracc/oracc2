#include <oraccsys.h>
#include "cfy.h"

static char **dash_split(char *l);

static Class defaults =
  {
    .key="cfy-noto-*-100-middle-osl",
    .fnt="noto",
    .otf="*",
    .mag="100",
    .scr="middle",
    .asl="osl"
  };

Class *curr_cp = &defaults;

static char **dash_split(char *l);

static void cfy_class_set(char *key, Class *cp);

/* Validate a key that has been split into dash-separated members */
static int
cfy_class_check_mem(const char *k, char **m)
{
  if (!m[0] || !m[1] || !m[2] || !m[3] || !m[4] || !m[5] || m[6])
    {
      fprintf(stderr, "cfy: bad key %s: wrong number of members\n", k);
      return 1;
    }
  
  /* fonts */
    
  /* otf sequence */
  
  /* magnification */
  
  /* script */
  
  /* sign list */

  return 0;
}

static char *
cfy_class_key(const char *fnt, const char *otf, const char *mag,
	      const char *scr, const char *asl)
{
  int len = strlen(fnt)+strlen(otf)+strlen(mag)+strlen(scr)+strlen(asl)+6;
  char *k = malloc(len);
  sprintf(k, "cfy-%s-%s-%s-%s-%s", fnt, otf, mag, scr, asl);
  return k;
}

static void
cfy_class_set(char *k, Class *cp)
{
  char **mem = dash_split(k);
  cp->fnt = mem[1];
  cp->otf = mem[2];
  cp->mag = mem[3];
  cp->scr = mem[4];
  cp->asl = mem[5];
  free(mem);
}

Class *
cfy_class(Cfy *c, const char *key, Class *cp)
{
  Class *ncp = NULL;
  if (key && *key)
    {
      char *kk = strdup(key);
      char **mem = dash_split(kk);
      
      if (cfy_class_check_mem(key, mem))
	{
	  free(mem);
	  free(kk);
	  return NULL;
	}
#define mcp(m,c) (*(m)=='*' ? (c) : (m))
      char *newkey = cfy_class_key(mcp(mem[1], cp ? cp->fnt : "*"),
				   mcp(mem[2], cp ? cp->otf : "*"),
				   mcp(mem[3], cp ? cp->mag : "*"),
				   mcp(mem[4], cp ? cp->scr : "*"),
				   mcp(mem[5], cp ? cp->asl : "*"));
#undef mcp
      if (!hash_find(c->hclasses, (uccp)newkey))
	{
	  const char *hk = (ccp)hpool_copy((uccp)newkey, c->hp);
	  ncp = memo_new(c->m_class);
	  cfy_class_set((char*)pool_copy((uccp)newkey, c->p), ncp);
	  hash_add(c->hclasses, (uccp)hk, ncp);
#if 0
	  if (!hash_find(c->hfonts, (uccp)ncp->fnt))
	    {
	      char ligf[strlen(oracc()) + strlen("/lib/data/ofs-.lig0") + strlen(ncp->fnt)];
	      sprintf(ligf, "%s/lib/data/ofs-%s.lig", oracc(), ncp->fnt);
	      ncp->lig = cfy_lig_load(ligf);
	      hash_add(c->hfonts, (uccp)ncp->fnt, ncp);
	    }
#endif	  
	}
      free(kk);
      free(mem);
      free(newkey);
    }

  return ncp;
}

static char **
dash_split(char *l)
{
  int ncol = 1;
  char *s = l;
  while (*s)
    {
      if ('-' == *s)
	++ncol;
      ++s;
    }
  if ('\n' == s[-1])
    s[-1] = '\0';

  char **p = calloc(ncol+1, sizeof(char *));
  int col;
  for (col = 0, s = l; *s; ++col)
    {
      if ('-' == *s)
	p[col] = "";
      else
	p[col] = s;
      while (*s && '-' != *s)
	++s;
      if ('-' == *s)
	*s++ = '\0';
    }

  return p;
}
