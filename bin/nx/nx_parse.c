#include <oraccsys.h>
#include "nx.h"

static int nxp_count_toks(const uchar **t);
static int nxp_last_num(const uchar **t, nx_numtok *n, int from);
static nx_result *nxp_new_result(int ntoks);
static int nxp_next_num(const uchar **t, nx_numtok *n, int from);
static void nxp_nonnums(nx_result *r, const uchar **toks, const void**data, int from, int to);
static nx_numtok nxp_tok_type(const uchar *t);

/*nx_parse works on a list of tokens which must be simple graphemes or
  unit-words.  Any other input must be preprocessed to meet this
  requirements, and ancillary data for each token can be provided in
  the data array.  The nx_result preserves the data pointer for any
  grapheme or unit-word that has it.
 */
nx_result *
nx_parse(const uchar **toks, const void **data, int ntoks)
{
  if (-1 == ntoks)
    ntoks = nxp_count_toks(toks);

  nx_result *r = nxp_new_result(ntoks);
  r->nptoks = calloc(ntoks, sizeof(nx_numtok));
  
  /* This loop iterates over the entire toks list */
  int start = 0;
  while (toks[start])
    {
      /* The inner loop finds the next num tok and the last num tok
	 regardless of system; parsing comes later */
      int next = nxp_next_num(toks, r->nptoks, start);
      if (next >= 0)
	{
	  if (start < next)
	    nxp_nonnums(r, toks, data, start, next);

	  /* last is the last token which is a number */
	  int last = nxp_last_num(toks, r->nptoks, next);

	  /* process the numbers */
	  nxp_numbers(r, r->nptoks, toks, data, next, last);

	  start = last+1;
	}
      else
	{
	  if (start < ntoks)
	    nxp_nonnums(r, toks, data, start, ntoks);
	  start = ntoks;
	}
    }
  
  return r;
}

static int
nxp_count_toks(const uchar **t)
{
  int n = 0;
  while (*t++)
    ++n;
  return n;
}

static
int nxp_last_num(const uchar **t, nx_numtok *n, int from)
{
  while (t[from])
    {
      n[from] = nxp_tok_type(t[from]);
      if (n[from] == nxt_no || n[from] == nxt_nz /* || n[from] == nxt_nc */)
	{
	  if (from)
	    return from-1;
	  else
	    return 0;
	}
      ++from;
    }
  return from-1;
}

static nx_result *
nxp_new_result(int ntoks)
{
  nx_result *r = calloc(1, sizeof(nx_result));
  
  /* preallocate enough space for one restok per tok */
  r->r = calloc(ntoks, sizeof(nx_restok));

  return r;
}

static int
nxp_next_num(const uchar **t, nx_numtok *n, int from)
{
  while (t[from])
    {
      n[from] = nxp_tok_type(t[from]);
      if (n[from] != nxt_no)
	return from;
      else
	++from;
    }
  return -1;
}

static void
nxp_nonnums(nx_result *r, const uchar **toks, const void**data, int from, int to)
{
  while (from < to)
    {
      r->r[r->nr].type = NX_NO;
      r->r[r->nr].no.tok = toks[from];
      if (data)
	r->r[r->nr].no.data = data[from];
      ++from;
      ++r->nr;
    }
}

static nx_numtok
nxp_tok_type(const uchar *t)
{
  int len = strlen((ccp)t);
  struct nxt_tab *ntp = NULL;
  if (!(ntp = nxt((ccp)t,len)))
    if (!(ntp = gc((ccp)t,len)))
      if (!(ntp = nc((ccp)t,len)))
	if (!(ntp = na((ccp)t,len)))
	  return nxt_no;
  return ntp->tok;
}
