#include <oraccsys.h>
#include "selib.h"

int estmangle = KM_REDUCE|KM_HYPHOK|KM_FOLD|KM_2VOWEL; 

struct est
{
  Hash *h;
  Pool *p;
  const char *project;
  const char *index;
  const char *filename;
  FILE *fp;
};

static void est_dump_one(const unsigned char *, struct est *);

void
est_add(const unsigned char *key, struct est *estp)
{
  const unsigned char *mangled_key = keymangler(key, estmangle, NULL, 0, NULL,NULL);
  hash_add(estp->h, pool_copy(mangled_key, estp->p), pool_copy(key, estp->p));
}

void
est_dump(struct est *estp)
{
  estp->fp = xfopen(estp->filename, "w");
  hash_exec_user_key(estp->h, (void (*)(const unsigned char *, void *))est_dump_one, estp);
  xfclose(estp->filename, estp->fp);
}

void
est_dump_one(const unsigned char *key, struct est *estp)
{
  fprintf(estp->fp, "%s\t%s\n", key, (unsigned char*)hash_find(estp->h, key));
}

struct est *
est_init(const char *project, const char *index)
{
  struct est *estp = malloc(sizeof(struct est));
  estp->h = hash_create(1000);
  estp->p = pool_init();
  estp->project = (const char*)pool_copy((const unsigned char *)project, estp->p);
  estp->index = (const char*)pool_copy((const unsigned char *)index, estp->p);
  estp->filename = (const char*)pool_copy((const unsigned char *)
					   se_file(project, index, "keys.est"), 
					   estp->p);
  return estp;
}

void
est_term(struct est *estp)
{
  hash_free(estp->h, NULL);
  pool_term(estp->p);
  free(estp);
}
