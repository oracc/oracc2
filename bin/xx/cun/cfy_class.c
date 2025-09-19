#include <oraccsys.h>
#include "cfy.h"

static char **dash_split(char *l);

static Class defaults =
  {
    .key="cfy-noto-*-100-middle-osl",
    .fnt="noto",
    .ffs="*",
    .mag="100",
    .scr="middle",
    .asl="osl"
  };

Class *curr_cp = &defaults;

static char **dash_split(char *l);

static void cfy_class_set(char *key, Class *cp);

/* Validate a key that has been split into dash-separated members */
static int
cfy_class_check(const char *k, char **m)
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
  int len = strlen(fnt)+strlen(otf)+strlen(mag)+strlen(scr)+strlen(asl)+strlen("cfy-----0");
  char *k = malloc(len);
  sprintf(k, "cfy-%s-%s-%s-%s-%s", fnt, otf, mag, scr, asl);
  return k;
}

static void
cfy_class_set(char *k, Class *cp)
{
  char **mem = dash_split(k);
  cp->fnt = mem[1];
  cp->ffs = mem[2];
  cp->mag = mem[3];
  cp->scr = mem[4];
  cp->asl = mem[5];
  if (cp->asl && '.' == *cp->asl)
    cp->asl = cp->cfyp->project;
  free(mem);
}

Fnt *
cfy_class_fnt(Cfy *c, Class *ncp)
{
  Fnt *fontp = hash_find(c->hfonts, (uccp)ncp->fnt);
  if (!fontp)
    {
      struct fontname *fp = fontname(ncp->fnt, strlen(ncp->fnt));
      if (fp)
	{
	  fontp = memo_new(c->m_fnt);
	  ncp->fntp = fontp;
	  hash_add(c->hfonts, (uccp)ncp->fnt, ncp->fntp);
	  fontp->name = ncp->fnt;
	  fontp->full = fp->full;
	  char ligf[strlen(oracc()) + strlen("/lib/data/.xxx0") + strlen(fontp->full)];
	  
	  sprintf(ligf, "%s/lib/data/%s.lig", oracc(), fontp->full);
	  ncp->fntp->ligs = cfy_lig_load(ligf);

	  if (c->coverage)
	    {
	      const char *uni = ligf;
	      if (c->cov_list)
		uni = c->cov_list;
	      else
		sprintf(ligf, "%s/lib/data/%s.uni", oracc(), fontp->full);
	      if ((ncp->fntp->uni = cfy_uni_load(c, uni)))
		ncp->fntp->uni_seen = hash_create(2048);
	    }
	}
      else
	fprintf(stderr, "cfy_class_fnt: no full font name for %s\n", ncp->fnt);
    }
  return fontp;
}

Class *
cfy_class(Cfy *c, const char *key, Class *cp)
{
  Class *ncp = NULL;
  if (key && *key)
    {
      char *kk = strdup(key);
      char **mem = dash_split(kk);
      
      if (cfy_class_check(key, mem))
	{
	  free(mem);
	  free(kk);
	  return NULL;
	}
#define mcp(m,c) (*(m)=='*' ? (c) : (m))
      char *newkey = cfy_class_key(mcp(mem[1], cp ? cp->fnt : "*"),
				   mcp(mem[2], cp ? cp->ffs : "*"),
				   mcp(mem[3], cp ? cp->mag : "*"),
				   mcp(mem[4], cp ? cp->scr : "*"),
				   mcp(mem[5], cp ? cp->asl : "*"));
#undef mcp
      if (!hash_find(c->hclasses, (uccp)newkey))
	{
	  ncp = memo_new(c->m_class);
	  ncp->cfyp = c;
	  ncp->key = (ccp)hpool_copy((uccp)newkey, c->hp); 
	  cfy_class_set((char*)pool_copy((uccp)ncp->key, c->p), ncp);
	  hash_add(c->hclasses, (uccp)ncp->key, ncp);
	  ncp->fntp = cfy_class_fnt(c, ncp);
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
