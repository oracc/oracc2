#include <oraccsys.h>
#include "bx.h"
#include "bib.h"

static void bnm_sH(void *userData, const char *name, const char **atts);
static void bnm_eH(void *userData, const char *name);

void
bnm_init(Bx *bp)
{
  bp->names = hash_create(1024);
  bp->akanames = hash_create(1024);
  bp->m_name = memo_init(sizeof(Name), 256);
  bnm_tab_init();
  const char *f[2] = { NULL , NULL };
  f[0] = oracc_data("bm-names.xml");
  runexpat_omit_rp_wrap();
  runexpatNSuD(i_list, f, bnm_sH, bnm_eH, NULL, bp);
}

Name *
bnm_lookup(Name *nmp)
{
  return NULL;
}

static const char *
bnm_make_name(Bx *bp, const char *last, const char *first)
{
  char buf[strlen(last)+strlen(first)+3];
  sprintf(buf, "%s, %s", last, first);
  return (ccp)pool_copy((uccp)buf, bp->p);
}

static Name *curr_name;
#define ubp	((Bx*)userData)
#define cdr(x)	curr_name->x=(ccp)pool_copy((uccp)charData_retrieve(),((Bx*)userData)->p)

static void
bnm_sH(void *userData, const char *name, const char **atts)
{
  struct bnm_tab *bnmp = bnm_tab(name, strlen(name));
  if (bnmp)
    (*bnmp->s)(userData, name, atts);
  else
    fprintf(stderr, "bnm_tab: unknown XML element %s\n", name);
}

static void
bnm_eH(void *userData, const char *name)
{
  struct bnm_tab *bnmp = bnm_tab(name, strlen(name));
  if (bnmp)
    (*bnmp->e)(userData, name);
  else
    fprintf(stderr, "bnm_tab: unknown XML element %s\n", name);  
}

void
bnm_s_namemap(void *userData, const char *name, const char **atts)
{
}

void
bnm_e_namemap(void *userData, const char *name)
{
}

void
bnm_s_name(void *userData, const char *name, const char **atts)
{
  curr_name = memo_new(ubp->m_name);
  curr_name->key = (ccp)pool_copy((uccp)findAttr(atts, "key"), ubp->p);
  curr_name->bm_name_xml = 1;
  hash_add(ubp->names, (uccp)curr_name->key, curr_name);
  if (verbose)
    fprintf(stderr, "bnm_s_name: curr_name = %s\n", curr_name->key);
}

void
bnm_e_name(void *userData, const char *name)
{
  curr_name = NULL;
}

void
bnm_s_last(void *userData, const char *name, const char **atts)
{
}

void
bnm_e_last(void *userData, const char *name)
{
  cdr(last);
}

void
bnm_s_rest(void *userData, const char *name, const char **atts)
{
}

void
bnm_e_rest(void *userData, const char *name)
{
  cdr(rest);
  hash_add(ubp->akanames,
	   (uccp)bnm_make_name(userData, curr_name->last, curr_name->rest),
	   curr_name);
}

void
bnm_s_aka(void *userData, const char *name, const char **atts)
{
}

void
bnm_e_aka(void *userData, const char *name)
{
  hash_add(ubp->akanames, pool_copy((uccp)charData_retrieve(),ubp->p), curr_name);
}

void
bnm_s_was(void *userData, const char *name, const char **atts)
{
}

void
bnm_e_was(void *userData, const char *name)
{
  cdr(was);
}

void
bnm_s_init(void *userData, const char *name, const char **atts)
{
}

void
bnm_e_init(void *userData, const char *name)
{
  cdr(init);
  hash_add(ubp->akanames,
	   (uccp)bnm_make_name(userData, curr_name->last, curr_name->init),
	   curr_name);
}

#undef cdr
#undef ubp
