#include <stdlib.h>
#include <stdio.h>
#include <oraccsys.h>
#include <list.h>
#include <oid.h>

static int32_t* oid_new_oids(Oids *o, int n);

int oo_xids = 0;
int oo_verbose = 0;

/* The assignees, k, have already been checked by oid_wants so we know
   that anything without an id needs one */
void
oid_assign(List *w, Oids *o)
{
  int need_oid = list_len(w);
  int32_t *newids = oid_new_oids(o, need_oid);
  int i = 0;
  struct oid *op;
  for (op = list_first(w); op; op = list_next(w))
    {
      static char buf[9];
      sprintf(buf, "%c%07d", (oo_xids ? 'x' : 'o'), newids[i++]);
      op->id = strdup(buf);
      fprintf(stderr, "adding %s with oid %s\n", op->key, op->id);
      hash_add(o->h_oid, (uccp)op->id, op);
      const char *dk = oid_domainify(op->domain, (ccp)op->key);
      hash_add(o->h_key, (uccp)strdup(dk), op);
    }
  free(newids);
}

const char *
oid_domainify(const char *d, const char *k)
{
  static char *buf = NULL;
  static int buflen = 0;
  if (!d && !k)
    {
      if (buf)
	{
	  free(buf);
	  buf = NULL;
	}
    }
  else
    {
      while (strlen(d)+strlen(k)+2 > buflen)
	buflen += 128;
      buf = realloc(buf, buflen);
      sprintf(buf, "%s:%s", d, k);
    }
  return buf;
}

struct oid_ok_pair *
oid_ok_pair(const char *oid, const char *key)
{
  struct oid_ok_pair *lp = calloc(1, sizeof(struct oid_ok_pair));
  lp->oid = oid;
  lp->key = key;
  return lp;
}

/* lp argument must be non-NULL */
struct oid_ok_pair *
oid_ok_pair_last(struct oid_ok_pair *lp)
{
  while (lp->next)
    lp = lp->next;
  return lp;
}

static int32_t *
oid_new_oids(Oids *o, int n)
{
  int i, j;
  int32_t *no = malloc(n*sizeof(int32_t));
  
  for (i = j = 0; o->lines[i+1] && j < n; ++i)
    {
      int32_t curr, next;
      curr = strtol((const char *)&o->lines[i][1], NULL, 10);
      next = strtol((const char *)&o->lines[i+1][1], NULL, 10);
      int gap = next - curr;
      while (gap-- > 1)
	no[j++] = ++curr;
    }
  int32_t last = strtol((const char *)&o->lines[i][1], NULL, 10);
  if (j < n)
    {
      while (j < n)
	no[j++] = ++last;
    }

  for (i = 0; i < n; ++i)
    fprintf(stderr, "oid_new_oids: %d\n", no[i]);
  
  return no;
}

#if 0
static int32_t
oid_next_idnum(Oids *o)
{
  static int i = 0;
  static int32_t prev = -1;
  static int32_t last_assigned = -1;
  for (; o->lines[i]; ++i)
    {
      int32_t curr;
      curr = strtol((const char *)&o->lines[i][1], NULL, 10);
      if (prev >= 0) {
	if (curr - prev > 1) /* this is a gap in numbering */
	  {    
	    last_assigned = prev + 1;
	    last_assigned;
	  }
      }
      last = this;
    }
  return last;
}

const char *
oid_next_oid(Oids *o)
{
  static char buf[9];
  int32_t n = oid_next_idnum(o);
  sprintf(buf, "%c%07d", (oo_xids ? 'x' : 'o'), n);
  return buf;
}
#endif

List *
oid_wants(Oids *o, Oids *k)
{
  size_t i;
  List *w = list_create(LIST_SINGLE);
  for (i = 0; i < k->nlines; ++i)
    {
      if (!k->oo[i]->id)
	{
	  const char *dk = oid_domainify(k->oo[i]->domain, (ccp)k->oo[i]->key);
	  struct oid *ho = hash_find(o->h_key, (uccp)dk);
	  if (ho)
	    k->oo[i]->id = ho->id;
	  else
	    list_add(w, k->oo[i]);
	}
    }
  return w;
}

void
oid_write(FILE *fp, Oids*o)
{
  int noids;
  const char **oids = hash_keys2(o->h_oid, &noids);
  qsort(oids, noids, sizeof(const char *), cmpstringp);
  size_t i;
  for (i = 0; i < noids; ++i)
    {
      Oid *op = hash_find(o->h_oid, (uccp)oids[i]);
      if (op->ext_type && !strcmp(op->ext_type, "word"))
	{
	  const char *dk = oid_domainify(op->domain, (ccp)op->key);
	  Oid *word_op = hash_find(o->h_key, (uccp)dk);
	  if (word_op)
	    op->extension = word_op->id;
	  else
	    fprintf(stderr, "oid_write: no OID for word %s\n", op->extension);
	}
      fprintf(fp, "%s\t%s\t%s\t%s\t%s\n",
	      op->id,
	      op->domain,
	      op->key,
	      op->type,
	      op->extension ? op->extension : "");
    }
}
