#include <oraccsys.h>
#include "cbd.h"

static List *cgplist = NULL;
static Pool *cgp_pool;

void
cgp_set_pool(Pool *p)
{
  cgp_pool = p;
}

List *
cgp_get_all(void)
{
  if (cgplist)
    {
      List *tmp = cgplist;
      cgplist = NULL;
      return tmp;
    }
  return NULL;
}

struct cgp*
cgp_get_one(void)
{
  if (cgplist)
    {
      struct cgp*ret = list_first(cgplist);
      list_free(cgplist, NULL);
      cgplist = NULL;
      return ret;
    }
  return NULL;
}

void
cgp_save(unsigned char *cf, unsigned char *gw, unsigned char *pos)
{
  struct cgp *c = memo_new(curr_cbd->cgpmem);
  cgp_init(c, cf, gw, pos);
  if (!cgplist)
    cgplist = list_create(LIST_SINGLE);
  list_add(cgplist, c);
}

void
cgp_init(struct cgp *c, unsigned char *cf, unsigned char *gw, unsigned char *pos)
{
  c->cf = cf;
  c->gw = gw;
  c->pos = pos;
  c->tight = cgp_str(c, 0);
  c->loose = cgp_str(c, 1);
}

const unsigned char *
cgp_str(struct cgp *cp, int loose)
{
  char *tmp = NULL;
  unsigned char*tmp2;

  if (!cp)
    return NULL;

  if (!cp->cf || !cp->gw || !cp->pos)
    return NULL;

  if (loose && cp->loose)
    return cp->loose;
  if (!loose && cp->tight)
    return cp->tight;
  
  tmp =  malloc(3+(loose*2)+strlen((ccp)cp->cf)+strlen((ccp)cp->gw)+strlen((ccp)cp->pos));
  if (loose)
    sprintf(tmp, "%s [%s] %s", cp->cf, cp->gw, cp->pos);
  else
    sprintf(tmp, "%s[%s]%s", cp->cf, cp->gw, cp->pos);
  tmp2 = pool_copy((ucp)tmp, cgp_pool);
  free(tmp);
  return tmp2;
}
