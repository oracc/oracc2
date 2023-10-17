#include <hash.h>
#include <pool.h>
#include <gdl.h>

static Hash *bridge_h = NULL;
static Pool *bridge_p = NULL;
extern int asl_flex_debug , gdl_flex_debug, nwarning;

#if 0
void
gvl_bridge_qpc(void)
{
  static int qpc_set = 0;
  if (!qpc_set)
    {
      gvl_i *gip = gvl_setup("pctc", "pctc", "900");
      langcore_set_sindex("900", gip->sindex);
      ++qpc_set;
    }
}
#endif

void
gvl_bridge_init(void)
{
  bridge_h = hash_create(1024);
  bridge_p = pool_init();
#if 0
  gvl_i *gip = gvl_setup("ogsl", "ogsl", "020");
  langcore_set_sindex("020", gip->sindex);
#endif
  gdlparse_init();
  mesg_init();
  gdl_flex_debug = 0;
  gvl_no_mesg_add = 1;
}

const char *
gvl_bridge(const char *f,size_t l,const unsigned char *g, int sindex)
{
  Tree *tp = NULL;
  static last_sindex = -1;

  if (sindex != last_sindex)
    last_sindex = gvl_switch(sindex);

  if (last_sindex == -1)
    return NULL;

#if 0
  const char *sentinel = "tumₓ(|TUM×GAN₂@t|)";
  if (!strcmp((ccp)g, sentinel))
    fprintf(stderr, "gvl_bridge: found %s\n", sentinel);
#endif
  
  tp = gdlparse_string(mesg_mloc(f,l), (char*)pool_copy(g,bridge_p));

  if (tp && tp->root && tp->root->kids)
    {
      gvl_g *gp = tp->root->kids->user;
      if (gp)
	{
	  hash_add(bridge_h, pool_copy(g,bridge_p), gp);
	  return (ccp)gp->mess;
	}
      else if (tp->root->kids->kids)
	{
	  /* sexified numbers like '1' have a user == NULL at
	     root->kids but the sexified version at
	     root->kids->kids */
	  if (!strcmp(tp->root->kids->kids->name, "g:n"))
	    {
	      gp = tp->root->kids->kids->user;
	      if (gp)
		return (ccp)gp->mess;
	    }
	}
    }
  
  mesg_verr(mesg_mloc(f,l), "(gvl_bridge) unknown error trying to parse %s", g);
  ++nwarning;
  return NULL;
}
