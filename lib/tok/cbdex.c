#include <oraccsys.h>
#include <tok.h>

#define cdup(s)	(ccp)hpool_copy((uccp)(s),cp->p)

struct cbdex_header *
cbdex_init(void)
{
  struct cbdex_header *chp = calloc(1, sizeof(struct cbdex_header));
  chp->p = hpool_init();
  chp->m = memo_init(sizeof(struct cbdex_header));
  return chp;
}

void
cbdex_term(struct cbdex_header *chp)
{
  memo_term(chp->m);
  hpool_term(chp->p);
  free(chp);
}

void
cbdex_new(struct cbdex_header *chp)
{
  return memo_new(chp->m);
}

void
cbdex_reset(struct cbdex *cp)
{
  memset(cp, '\0', sizeof(struct cbdex));
}

void
cbdex_loc(char *lp, struct cbdex *cp)
{
  char **f = tab_split(lp);
  if ('A' == *lp) /* article */
    {
      cp->lem = cdup(f[2]);
      cp->oid = cdup(f[1]);
      cp->lcnt = cdup(f[3]);
    }
  else if ('B' == *lp) /* base */
    {
      cp->base = cdup(f[2]);
      cp->bcnt = cdup(f[3]);
    }
  else
    fprintf(stderr, "cbdex_loc: internal error: unhandled location code '%c'\n", *lp);
  free(f);
}

void
cbdex_g(char *g, struct cbdex *cp)
{
  if ('g' == *g)
    {
      char **f = tab_split(g);
      cp->tok = cdup(f[1]);
      cp->gpos = cdup(gsig_no_d_pos(f[2]));
      free(f);
    }
  else
    fprintf(stderr, "cbdex_loc: internal error: unhandled grapheme code '%c'\n", *g);
}

/* Caller must ensure the string c is allocated appropriately */
void
cbdex_parse(char *c, struct cbdex *cp)
{
  char **f = tab_split(c);
  cp->tok = f[0];
  cp->lem = f[1];
  cp->oid = f[2];
  cp->lcnt = f[3];
  cp->base = f[4];
  cp->bcnt = f[5];
  cp->gpos = f[6];
  free(f);
}

void
cbdex_write(FILE *fp, struct cbdex *cp)
{
  fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
	  cp->tok,
	  cp->lem,
	  cp->oid,
	  cp->lcnt,
	  cp->base,
	  cp->bcnt,
	  cp->gpos
	  );
}

