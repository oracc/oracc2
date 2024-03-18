#include <oraccsys.h>
#include <xpd.h>
#include <runexpat.h>
#include "selib.h"

struct xpd *xp = NULL;
Pool *sepool;

extern struct lm_tab *langmask(register const char *str, register unsigned int len);

#if 0
static void
se_xpd(const char *proj)
{
  if (!proj)
    {
      if (sepool)
	pool_term(sepool);
      if (xp)
	xpd_term(xp);
      sepool = NULL;
      xp = NULL;
      return;
    }
  else if (!sepool)
    sepool = pool_init();
  if (!xp)
    xp = xpd_init(proj, sepool);
}
#endif

const char *
attr_by_name(const char **atts,const char *name)
{
  int i;
  for (i = 0; atts[i] != NULL; i+=2)
    if (!strcmp(atts[i],name))
      return atts[i+1];
  return NULL;
}

const char *
xml_id(const char **atts)
{
  const char *x = get_xml_id(atts);
  if (x && *x)
    return x;
  else
    return NULL;
}

const char *
xml_lang(const char **atts)
{
  const char *x = get_xml_lang(atts);
  if (x && *x)
    return x;
  else
    return NULL;
}

int
lang_mask(const char **atts)
{
  const char *langsp = langs(atts);
  if (langsp)
    return strtoul(langsp,NULL,0);
  else
    return 0;
}

void
loc8(const char *id, int word, int lmask, struct location8 *l8p)
{
  l8p->text_id = l8p->unit_id = l8p->word_id = 0;
  l8p->text_id = atoi(id+1);
  if (*id == 'Q')
    setQ(l8p->text_id);
  else if (*id == 'X')
    {
#if 1
      l8p->text_id = XIFY_ID(l8p->text_id);
#else
      int orig = l8p->text_id;
      l8p->text_id = XIFY_ID(l8p->text_id);
      fprintf(stderr,"remapping text_id %c%d to %d=0x%x\n",*id,orig,l8p->text_id,l8p->text_id);
#endif
    }
  l8p->text_id |= lmask;
}

int
lang2mask(const char *l)
{
  struct lm_tab *tabp = langmask(l,strlen(l));
  if (tabp)
    return tabp->lm;
  else
    return lm_misc;
}
