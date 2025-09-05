#include <oraccsys.h>
#include "cfy.h"

#define CFY_SUB_MAX	1024

static Subspec *
sub_match(Cfy *c, Elt **epp, int i, Subspec *hsp)
{
  char buf[CFY_SUB_MAX];
  int j;
  Subspec *msp = (hsp->terminal ? hsp : NULL); /* matching Subspec */
  strcpy(buf, epp[i]->key);
  if (trace && msp)
    fprintf(stderr, "trace: sub_match() entered with single-elt terminal %s\n", buf);
  for (j = i+1; epp[j]; ++j)
    {
      const char *k = elts_one_key(epp[j]);
      if (k) /* deleted nodes return NULL so they get ignored */
	{
	  if (strlen(buf) + strlen(k) >= CFY_SUB_MAX-3)
	    {
	      fprintf(stderr, "cfy: key too long\n"); /* not good enough */
	      return NULL;
	    }
	  else
	    {
	      if (*buf)
		strcat(buf, "_");
	      strcat(buf, k);
	      if (trace)
		fprintf(stderr, "trace: sub_match trying hsubkeys for %s\n", buf);
	      Subspec *sp = hash_find(c->hsubkeys, (uccp)buf);
	      if (sp)
		{
		  if (sp->terminal)
		    msp = sp; /* last longest match but keep trying because match is greedy */
		  if (trace)
		    fprintf(stderr,
			    "trace: sub_match found %s %s\n",
			    sp->terminal ? "terminal" : "subpath", buf);
		}
	      else
		break; /* quit on first match failure */
	    }
	}
    }
  /* the last successful match, if any, is now in msp */
  return msp;
}

/* For ELT_G types, import class, oid, xid from original data to replacement sp->r */
static Elt **
sub_r_fix(Cfy *c, Eltline *elp, int i, Subspec *sp)
{
  Elt **nr = memo_new_array(c->m_subspec, sp->r_len);
  Elt *o_g = elp->epp[i]; /* matches always start with ELT_G */
  memcpy(nr, sp->r, sizeof(Elt*) * sp->r_len);
  int j;
  for (j = 0; j < sp->r_len; ++j)
    {
      /* look for a match to k in elp->epp starting at elt j */
      if (sp->r[j]->key)
	{
	  int k = 0;
	  Elt *match = NULL;
	  while (k < sp->l_len)
	    {
	      if (elp->epp[i+k]->key)
		{
		  if (!strcmp(elp->epp[i+k]->key, sp->r[j]->key))
		    {
		      match = elp->epp[i+k];
		      break;
		    }
		}
	      ++k;
	    }
	  /* if found, use the info from that match */
	  if (match)
	    *nr[j] = *match;
	  else
	    {
	      /* state inheritance has to be by individual members here
		 oid, data, key all need to be set when the sub rhs element is created
	       */
	      nr[j]->c = o_g->c;
	      nr[j]->btype = o_g->btype;
	      nr[j]->xid = o_g->xid;
	    }
	}
    }
  return nr;
}

static Elt **
sub_replace(Cfy *c, Eltline *elp, int i, Subspec *sp, int *ip)
{
  Elt**dest = elp->epp;
  Elt **new_r = sub_r_fix(c, elp, i, sp);
  if (sp->r_len > sp->l_len)
    {
      int nlen = elp->len + (sp->r_len - sp->l_len) + 1;
      int last_seg = elp->len - (i+sp->l_len); 	/* Elt* after the end
						   of the replaced
						   segment */
      dest = memo_new_array(c->m_elt, nlen);
      memcpy(dest, elp->epp, i * sizeof(Elt*));
      memcpy(dest+i, new_r, sp->r_len * sizeof(Elt*));
      memcpy(dest+i+sp->r_len, elp->epp+i+sp->l_len, last_seg * sizeof(Elt*));
      *ip = nlen;      
    }
  else if (sp->r_len == sp->l_len)
    {
      memcpy(dest+i, new_r, sp->r_len * sizeof(Elt*));
    }
  else
    {
      int last_seg = elp->len - (i+sp->l_len); 	/* Elt* after the end
						   of the replaced
						   segment */
      memcpy(dest+i, new_r, sp->r_len * sizeof(Elt*));
      memmove(dest+i+sp->r_len, elp->epp+i+sp->l_len, last_seg * sizeof(Elt*));
      *ip = i + sp->r_len + last_seg;
      dest[*ip] = NULL;
    }
  
  return dest;
}

void
cfy_subbings(Cfy *c, Eltline *elp)
{
  int i;
  for (i = 0; elp->epp[i]; )
    {
      Subspec *hsp;
      if (elp->epp[i]->key && (hsp = hash_find(c->hsubkeys, (uccp)elp->epp[i]->key)))
	{
	  Subspec *sp = sub_match(c, elp->epp, i, hsp);
	  /* because this does not increment i we should implement an
	     infinite-loop trap because multiple subspecs could
	     recreate matches for each other */
	  if (sp)
	    elp->epp = sub_replace(c, elp, i, sp, &elp->len);
	  else
	    ++i;
	}
      else
	++i;
    }
}
