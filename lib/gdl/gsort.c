#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tree.h>
#include <memo.h>
#include <gutil.h>
#include <collate.h>
#include "gdl.h"
#include "gsort.h"

/**
 *
 * gsort--implement the Oracc Graphemic Sort as described in
 *
 * 	http://oracc.org/osl/Sorting
 *
 */

int gsort_trace = 0; /* if this is non-zero the item structures for
			each sequence are printed on stderr */

/**
 *
 * Logistics management
 *
 */

static GS_item *gsort_item(int type, unsigned const char *n, unsigned const char *g, unsigned const char *r);
static GS_mods *gsort_mods(unsigned char *m);
static const char *gsort_show_key(unsigned const char *k);
static int gsort_cmp_item(GS_item *a, GS_item *b);
static int gsort_cmp_mods(GS_mods *ap, GS_mods *bp);
static void gsort_node(Node *np, List *lp);
static void gsort_reset_c_type(GS_head *gs);

static Memo *m_headers = NULL;
static Memo *m_items = NULL;  /* GS_item   */
static Memo *m_itemps = NULL; /* GS_item*  */
static Memo *m_mods = NULL;
static Pool *gspool = NULL;
static Hash *hitems = NULL;
static Hash *hheads = NULL;

static GS_item gsort_null_item = { (uccp)"\\0" , 0, (uccp)"", (uccp)"", (uccp)"", NULL, (uccp)"", 0, -1 };

/* Initialize storage and structures */
void
gsort_init()
{
  m_headers = memo_init(sizeof(GS_head), 1024);
  m_items = memo_init(sizeof(GS_item), 1024);
  m_itemps = memo_init(sizeof(GS_item*), 1024);
  m_mods = memo_init(sizeof(GS_mods), 1024);
  gspool = pool_init();
  hitems = hash_create(1024);
  hheads = hash_create(1024);
  collate_init((ucp)"unicode");
}

/* Terminate storage and structures */
void
gsort_term()
{
  memo_term(m_headers);
  memo_term(m_items);
  memo_term(m_itemps);
  pool_term(gspool);
  hash_free(hheads, NULL);
  hash_free(hitems, NULL);
  if (gsort_trace)
    gsort_show_key(NULL);
}

/**
 *
 * Preparing sequences for sorting
 *
 */

/* Prepare a sequence for sorting by calling gsort_node on each of the
   GDL nodes to create list of the sort-items in the sequence header */
GS_head *
gsort_prep(Tree *tp)
{
  if (tp && tp->root)
    {
      GS_head *gs = memo_new(m_headers);

      if (tp->root->text && (gs = hash_find(hheads, (uccp)tp->root->text)))
	return gs;
      else
	{
	  List *lp = list_create(LIST_SINGLE);
	  gs = memo_new(m_headers);
	  if (tp->root->kids)
	    {
	      Node *np;
	      for (np = tp->root->kids; np; np = np->next)
		gsort_node(np, lp);

	      gs->n = list_len(lp);
	      gs->i = (GS_item **)list2array(lp);
	      list_free(lp, NULL);
	      if (tp->root->kids)
		{
		  gs->s = (uccp)tp->root->text;
		  if ('|' == *gs->s && gs->i[0]->t == 2)
		    gsort_reset_c_type(gs);
		}

	      hash_add(hheads, gs->s, gs);
	    }
	  else if (tp->root->text)
	    {
	      GS_item *gi = gsort_item(0, (uccp)tp->root->text, (uccp)tp->root->text, NULL);
	      gs->n = 1;
	      gs->i = memo_new(m_itemps);
	      *gs->i = gi;
	      gs->s = (uccp)tp->root->text;
	      hash_add(hheads, gs->s, gs);
	    }
	  else
	    gs->s = NULL;
	  return gs;
	}
    }
  return NULL;
}

/* Process a single node based on type, calling gsort_item to create
   the item structure and adding the item to the list of comparands */
static void
gsort_node(Node *np, List *lp)
{
  static int pending_r = 0;
  
  /* Ignore determinatives for now; should probably ensure that forms
     with determinative sort in constant order */
  if (!strcmp(np->name, "g:det"))
    return;

  switch (np->name[2])
    {
    case 'p':
      list_add(lp, gsort_item(1, (uccp)np->text, (uccp)np->text, NULL));
      break;
    case 'l':
    case 's':
    case 'v':
      list_add(lp, gsort_item(0, (uccp)np->text, (uccp)np->text, NULL));
      if (pending_r)
	{
	  GS_item *gp = list_pop(lp);
	  gp->r = pending_r;
	  list_add(lp, gp);
	  /* add a null delimiter in lieu of TIMES to coax 4×ZA into sorting after ZA */
	  list_add(lp, &gsort_null_item);
	  pending_r = 0;
	}
      break;
    case 'n':
      list_add(lp, gsort_item(2, (uccp)np->text,
			      np->kids ? (uccp)np->kids->next->text : (uccp)np->text,
			      np->kids ? (uccp)np->kids->text : (uccp)np->text));
      break;
    case 'd':
      /* For d = '3×' store the multiplier and don't add the TIMES */
      if (*np->text < 128 && isdigit(*np->text))
	pending_r = atoi(np->text);
      else
	list_add(lp, gsort_item(0, (uccp)np->text, (uccp)np->text, NULL));
      break;
    case 'a':
    case 'm':
      list_add(lp, &gsort_null_item);
      list_add(lp, gsort_item(0, (uccp)np->text, (uccp)np->text, NULL));
      break;
    case 'b':
    case 'c':
    case 'g':
      {
	Node *npp;
	for (npp = np->kids; npp; npp = npp->next)
	  gsort_node(npp, lp);
      }
      break;
    case 'q':
      /* for qualified signs use the value, a NULL delimiter (which
	 will sort before other delimiters) and the sign */
      list_add(lp, gsort_item(0, (uccp)np->text, (uccp)np->kids->text, NULL));
      list_add(lp, &gsort_null_item);
      {
	Node *npp;
	for (npp = np->kids->next; npp; npp = npp->next)
	  gsort_node(npp, lp);
      }
      break;
    default:
      fprintf(stderr, "[gsort] unhandled name in gsort_node %s\n", np->name);
      break;
    }
}

/* Fill out an item structure for a grapheme or delimiter; modifiers
   have special handling in gsort_mods */
static GS_item *
gsort_item(int type, unsigned const char *n, unsigned const char *g, unsigned const char *r)
{
  GS_item *gp = NULL;
  unsigned char *tmp;

  if ((gp = hash_find(hitems, n)))
    return gp;

  gp = memo_new(m_items);
  gp->t = type;
  gp->g = n;
  tmp = pool_copy(g_base_of(g), gspool);
  gp->b = tmp;

  gp->x = g_index_of(g, gp->b);

  if ((tmp = (ucp)strpbrk((ccp)gp->b, "~@")))
    {
      gp->m = pool_copy(tmp, gspool);
      gp->mp = gsort_mods(tmp);
      *tmp = '\0';
    }
  else
    gp->m = (ucp)"";

  /* Reset Punct/Num types for graphemes that aren't tagged g:p/g:n in
     GDL; gp->b (BASE) is lowercase by now */
  if (!strcmp((ccp)gp->b, "p"))
    gp->t = 1;
  else if ('n' == *gp->b || 'f' == *gp->b) /* F is F(raction) */
    {
      unsigned const char *b = gp->b + 1;
      while (*b)
	if (*b > 128 || !isdigit(*b))
	  break;
	else
	  ++b;
      if (!*b)
	gp->t = 2;
    }
  else if ((tmp = (ucp)strpbrk((ccp)gp->b, "0123456789")))
    {
      /* gsort considers a non-N-matching pattern /^(.+)([0-9]+)(.*)$/
	 as a list--even it it's not a list it gets the list
	 treatment, which is to set the BASE to \1; the index to \2;
 	 and the suffix, if any, to gp->s

	 lists have the same type as regular graphemes but can be
	 differentiated because INDEX is >=10000.
       */
      gp->x = atoi((ccp)tmp) + 10000;
      *tmp++ = '\0';
      while (*tmp && *tmp < 128 && isdigit(*tmp))
	++tmp;
      if (*tmp)
	gp->s = tmp;
    }
  
  gp->k = collate_makekey(pool_copy(gp->b, gspool));

  if (r)
    {
      if ('n' == *r)
	gp->r = 1000;
      else if ('N' == *r)
	gp->r = 2000;
      else
	gp->r = atoi((ccp)r);
    }
  else if (gp->t == 2)
    gp->r = 0;
  else
    gp->r = -1;

  hash_add(hitems, n, gp);
  
  return gp;
}

static GS_mods *
gsort_mods(unsigned char *m)
{
  GS_mods *ret = NULL, *curr = NULL;
  char *s = (char *)m;
  while (*s)
    {
      if ('~' == *s || '@' == *s)
	{
	  GS_mods *mp = memo_new(m_mods);
	  if (!curr)
	    ret = curr = mp;
	  else
	    {
	      curr->next = mp;
	      curr = curr->next;
	    }
	  mp->type = *s;
	  *s++ = '\0';
	  mp->val = s;	  
	}
      else
	++s;
    }
  return ret;
}

/* if the tree text is a compound and the first element
   is type 2, number, check to see if there are any
   non-numbers in the compound; if there are, reset the
   head type to be a regular grapheme--this is so that
   |1(N57).ŠAH₂| sorts with regular graphemes rather
   than numbers */
static void
gsort_reset_c_type(GS_head *gs)
{
  int non_num = 0, i;
  for (i = 0; i < gs->n; ++i)
    if (gs->i[i]->t == 0)
      ++non_num;
  if (non_num)
    for (i = 0; i < gs->n; ++i)
      if (gs->i[i]->t == 2)
	gs->i[i]->t = 0;
}

/**
 *
 * Comparing sequences
 *
 */

/* This is the controller routine that should be passed to qsort
 *
 * Compare the sequences one item at a time using gsort_cmp_item. */
int
gsort_cmp(const void *v1, const void *v2)
{
  GS_head *h1 = *(GS_head**)v1;
  GS_head *h2 = *(GS_head**)v2;
  int i, ret;
  
  for (i = 0; i < h1->n && i < h2->n; ++i)
    if ((ret = gsort_cmp_item(h1->i[i], h2->i[i])))
      return ret;

  if (h1->i[i])
    return 1;
  else if (h2->i[i])
    return -1;

  /* if we are still here the grapheme sequences are identical when ignoring case.
     
     sll_has_sign_indicator looks for key uppercase letters to detect
     sign names so we use that to try to resolve via lower/upper case
   */
  if ((ret = strcmp((ccp)h1->s, (ccp)h2->s)))
    {
      if (sll_has_sign_indicator(h1->s))
	return 1;
      else if (sll_has_sign_indicator(h2->s))
	return -1;
      else
	return ret;
    }

  return 0;
}

/* Compare items according to the Oracc Grapheme Sort algorithm; mods
 * get special treatment with gsort_cmp_mods */
static int
gsort_cmp_item(GS_item *a, GS_item *b)
{
  int ret = 0;

  /* compare type */
  if (a->t != b->t)
    return a->t - b->t;

  /* compare key */
  if ((ret = strcmp((ccp)a->k, (ccp)b->k)))
    return ret;

  /* compare index */
  if (a->x - b->x)
    return a->x - b->x;

  /* compare suffix */
  if (a->s || b->s)
    {
      if (a->s && b->s)
	{
	  if ((ret = strcmp((ccp)a->s,(ccp)b->s)))
	    return ret;
	}
      else
	return a->s ? 1 : -1;
    }
  
  /* compare mods */
  if (a->mp && b->mp)
    {
      if ((ret = gsort_cmp_mods(a->mp,b->mp)))
	return ret;
    }
  else if (a->mp || b->mp)
    return a->mp ? 1 : -1;

  /* compare repeat */
  if (a->t == 2)
    return a->r - b->r;
  else
    /* see if this is |3×AN| or like */
    if (a->t == 0 && (a->r > 0 || b->r > 0) && a->r != b->r)
      return (a->r > 0) ? 1 : -1;

  /* items are identical when ignoring case */
  return 0;
}

static int
gsort_cmp_mods(GS_mods *ap, GS_mods *bp)
{
  while (1)
    {
      if (ap->type != bp->type)
	return ('@' == ap->type) ? 1 : -1;
      int ret = strcmp(ap->val, bp->val);
      if (ret)
	return ret;
      if (ap->next && bp->next)
	{
	  ap = ap->next;
	  bp = bp->next;
	}
      else if (ap->next)
	return 1;
      else if (bp->next)
	return -1;
      else
	return 0;
    }
}

/**
 *
 * Debugging display routines
 *
 */

void
gsort_show(GS_head *gsp)
{
  if (gsp)
    {
      int i;
      fputs((ccp)gsp->s, stderr);
      fputc('\t', stderr);
      for (i = 0; i < gsp->n; ++i)
	{
	  GS_item *gip = gsp->i[i];
	  fprintf(stderr, "{%s; %d; %s; %s; %s; %s",
		  gip->g, gip->t, gip->b, gsort_show_key(gip->k), gip->s ? gip->s : (uccp)"", gip->m);
	  if (gip->mp)
	    {
	      GS_mods *mp;
	      fputs(" [", stderr);
	      for (mp = gip->mp; mp; mp = mp->next)
		{
		  fprintf(stderr, "%c%s",mp->type,mp->val);
		  if (mp->next)
		    fputc(',', stderr);
		}
	      fputc(']', stderr);
	    }
	  fprintf(stderr, "; %d; %d}", gip->x, gip->r);
	}
      fputc('\n', stderr);
    }
  else
    fprintf(stderr, "(GS_head argument is NULL)\n");
}

static const char *
gsort_show_key(unsigned const char *k)
{
  static char *kk = NULL;
  char *kp;
  static int kk_len = 0;
  int kk_need = 0;

  if (!k)
    {
      kk_len = 0;
      free(kk);
      return (kk = NULL);
    }

  kk_need = (2 * strlen((ccp)k))+1;

  if (kk_need > kk_len)
    kk = realloc(kk, 2*kk_need);
  
  for (kp = kk; *k;)
    {
      if (*k < 32)
	{
	  *kp++ = '\\';
	  *kp++ = (*k++ + 'a');
	}
      else
	*kp++ = *k++;
    }
  *kp = '\0';
  
  return kk;
}
