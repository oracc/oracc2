#include "ax.h"

struct run_context *
run_init(void)
{
  struct run_context *ret = calloc(1, sizeof(struct run_context));
  ret->known_projects = hash_create(1);
  ret->pool = pool_init();
  return ret;
}

void
run_term(struct run_context *rp)
{
  pool_term(rp->pool);
  hash_free(rp->known_projects, (hash_free_func*)proj_term);
  free(rp);
}
