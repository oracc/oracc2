#include <hash.h>
#include <pool.h>
#include <gdl.h>

static Hash **bridge_a = NULL;
/*static Hash *bridge_h = NULL;*/
static Pool *bridge_p = NULL;
extern int asl_flex_debug , gdl_flex_debug, nwarning;
static gvl_g *gbgp;

const unsigned char *
gvl_bridge_atf2utf(void)
{
  if (gbgp)
    return gbgp->orig;
  else
    return NULL;
}

unsigned const char *
gvl_bridge_cuneify(void)
{
  return gvl_cuneify_gv(gbgp);
}

unsigned const char *
gvl_bridge_key()
{
  if (gbgp && gbgp->oid)
    {
      char buf[strlen((ccp)gbgp->orig)+14];
      sprintf(buf, "%s.%s;fvp", gbgp->oid, gbgp->orig);
      const unsigned char *fvp = gvl_lookup((uccp)buf);
      if (!fvp)
	{
	  sprintf(buf, "%s;default", gbgp->oid);
	  fvp = gvl_lookup((uccp)buf);
	  if (fvp)
	    sprintf(buf, "%s.", gbgp->oid);
	}
      if (fvp)
	{
	  char key[strlen((ccp)buf)+10];
	  sprintf(key, "%s.%s", fvp, buf);
	  char *s = strchr(key, ';');
	  if (s)
	    *s = '\0';
	  return pool_copy((uccp)key, bridge_p);
	}
      else
	{
	  char key[strlen((ccp)gbgp->orig)+14];
	  sprintf(key, "%s..%s", gbgp->oid, gbgp->orig);
	  return pool_copy((uccp)key, bridge_p);
	}
    }
  else
    return NULL;
}

unsigned const char *
gvl_bridge_oid_name(const char *oid)
{
  return gvl_lookup((uccp)oid);
}

const char *
gvl_bridge_oid(void)
{
  if (gbgp)
    return gbgp->oid;
  else
    return NULL;
}

const char *
gvl_bridge_spoid(void)
{
  if (gbgp)
    return (ccp)pool_copy((uccp)gbgp->sp_oid, bridge_p);
  else
    return NULL;
}

const unsigned char *
gvl_bridge_signname(void)
{
  if (gbgp)
    return gbgp->sign;
  else
    return NULL;
}

void
gvl_bridge_init(void)
{
  /* This is just enough to work with oxx because it assumes just
     osl/pcsl as possible sl */
  bridge_a = malloc(3 * sizeof(Hash*));
  bridge_a[1] = hash_create(1024);
  bridge_a[2] = hash_create(1024);
  bridge_p = pool_init();
  gdlparse_init();
  mesg_init();
  gdl_flex_debug = 0;
  gvl_no_mesg_add = 1;
}

const char *
gvl_bridge(const char *f,size_t l,const unsigned char *g, int sindex)
{
  Tree *tp = NULL;
  static int last_sindex = -1;
  
  if (sindex != last_sindex)
    last_sindex = gvl_switch(sindex);

  if (last_sindex == -1)
    return NULL;

  if ((gbgp = hash_find(bridge_a[sindex], g)))
    return (const char *)gbgp->mess;
  
  tp = gdlparse_string(mesg_mloc(f,l), (char*)pool_copy(g,bridge_p));

  if (tp && tp->root && tp->root->kids)
    {
      gbgp = tp->root->kids->user;
      if (gbgp)
	{
	  hash_add(bridge_a[sindex], pool_copy(g,bridge_p), gbgp);
	  return (ccp)gbgp->mess;
	}
      else if (tp->root->kids->kids)
	{
	  /* sexified numbers like '1' have a user == NULL at
	     root->kids but the sexified version at
	     root->kids->kids */
	  if (!strcmp(tp->root->kids->kids->name, "g:n"))
	    {
	      gbgp = tp->root->kids->kids->user;
	      if (gbgp)
		return (ccp)gbgp->mess;
	    }
	}
      else if (!strcmp(tp->root->kids->name, "g:B")
	       && !strcmp(tp->root->kids->text, "0"))
	{
	  /* This is a bug in GVL--unadorned '0' is not sexified
	     properly; for now we ignore it */
	  return NULL;
	}
    }
  
  mesg_verr(mesg_mloc(f,l), "(gvl_bridge) unknown error trying to parse %s", g);
  ++nwarning;
  return NULL;
}
