#include <unistd.h>
#include <oraccsys.h>
#include <runexpat.h>
#include <string.h>
#include "xmd.h"

static Hash *xmd_vals = NULL;
static int in_cat_data;
static Pool * xmd_pool = NULL;

void
xmd_init(void)
{
  if (xmd_vals)
    {
      hash_free(xmd_vals, NULL);
      if (xmd_pool)
	pool_term(xmd_pool);
    }
  
  xmd_vals = hash_create(1);
  xmd_pool = pool_init();
}

void
xmd_term(void)
{
  if (xmd_vals)
    {
      hash_free(xmd_vals,NULL);
      xmd_vals = NULL;
    }
  if (xmd_pool)
    {
      pool_term(xmd_pool);
      xmd_pool = NULL;
    }
}

static void
xmd_sH(void *userData, const char *name, const char **atts)
{
  if (!in_cat_data)
    {
      if (!strcmp(name,"cat"))
	in_cat_data = 1;
    }
  else if (!strcmp(name,"images"))
    in_cat_data = 0;
  charData_discard();
}

static void
xmd_eH(void *userData, const char *name)
{
  if (in_cat_data) /* breaks if a key can occur more than once */
    {
      if (!strcmp(name,"cat"))
	in_cat_data = 0;
      else if (strcmp(name,"subfield")) /*FIXME: should do something
					  with subfields */
	hash_add(xmd_vals, 
		 (unsigned char*)pool_copy((ucp)name, xmd_pool), 
		 (unsigned char *)pool_copy((ucp)charData_retrieve(), xmd_pool));
    }
  else if (!strcmp(name,"images"))
    in_cat_data = 1;
}

Hash *
xmd_load(const char *project, const char *pq)
{
  const char *fn = NULL;
  const char *fns[2];
  char *tmp = NULL, *nproj, *npq;
  
  if (!project && (strchr(pq,':')))
    {
      tmp = malloc(strlen(pq)+1);
      strcpy(tmp,pq);
      nproj = npq = tmp;
      npq = strchr(npq, ':');
      *npq++ = '\0';
      fn = expand(nproj,npq,"xmd");
      free(tmp);
    }
  else
    fn = expand(project,pq,"xmd");

  fns[0] = fn;
  fns[1] = NULL;
  in_cat_data = 0;
  runexpat(i_list, fns, xmd_sH, xmd_eH);
  return xmd_vals;
}
