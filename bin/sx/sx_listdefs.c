#include <signlist.h>
#include <sx.h>

void
sx_listdefs_sort(struct sl_listdef *ldp)
{
  qsort(ldp->names, ldp->nnames, sizeof(const char *), via_tok_cmp);
}

static void
sx_listdefs_one(struct sl_signlist *sl, const char *listname)
{
  struct sl_listdef *ldp = hash_find(sl->listdefs, (uccp)listname);

  if (ldp)
    {
      /* sort the list's entries if necessary */
      if (!ldp->sorted++)
	sx_listdefs_sort(ldp);
      int i;
      for (i = 0; i < ldp->nnames; ++i)
	if (!hash_find(ldp->seen, (uccp)ldp->names[i]))
	  fprintf(stdout,"%s\n", ldp->names[i]);
    }
  else
    mesg_verr(&sl->mloc, "request to check missing items in unknown list %s", listname);
}

/**sx_ld_count
 *
 * Count commas in string, counting sequences of multiple commas as a
 * single comma.
 */
static int
sx_ld_count(const char *n)
{
  int i;
  for (i = 0; *n; ++i)
    {
      while (',' == *n)
	++n;
      while (*n && ',' != *n)
	++n;
    }
  return i;
}

/**sx_listdefs
 *
 * At the end of signlist processing, if -M or -m LISTS is given,
 * report on which listnums were declared in listdefs but didn't occur
 * in the signlist.
 *
 * argument listnames is a comma-separated list of names
 */
void
sx_listdefs(struct sl_signlist *sl, const char *listnames)  
{
  const char **names;
  int nnames;
  if (listnames)
    {
      int ncomma = sx_ld_count(listnames), i;
      char *n, *ln;
      ln = (char*)pool_copy((uccp)listnames, sl->p);
      names = malloc((ncomma+1) * sizeof(const char *));
      names[0] = ln;
      for (n = ln, i = 1; *n; ++n)
	{
	  if (',' == *n)
	    {
	      *n++ = '\0';
	      /* trap listnames ending with comma */
	      while (',' == *n)
		++n;
	      if ('\0' == *n)
		break;
	      names[i++] = n;
	    }
	}
      names[i] = NULL;
      nnames = i;
    }
  else
    {
      names = hash_keys2(sl->listdefs, &nnames);
    }

  qsort(names, nnames, sizeof(const char *), cmpstringp);  
  
  int i;
  for (i = 0; names[i]; ++i)
    sx_listdefs_one(sl, names[i]);

  free(names);
}

static void
sx_list_row(FILE *f, struct sl_signlist *sl, const unsigned char *name,
	    struct sl_inst *ip, struct sl_token *tp,
	    struct sl_unicode *up, struct sl_list *lp,
	    const unsigned char *note)
{
#define nonull(s) (ccp)(s ? (ccp)s : "")

  const char *code = "";
  const unsigned char *ucun = (uccp)"";
 
  if (up)
    {
      if (up->uhex)
	code = up->uhex;
      else if (up->upua)
	code = up->upua;
      else if (up->useq)
	code = up->useq;
      if (up->utf8)
	ucun = up->utf8;
    }

  int sf_sort = 0;
  unsigned const char *sf_name = (uccp)"";
  const char *oid = "";
  if (ip)
    {
      if (ip->type == 's')
	{
	  oid = ip->u.s->oid;
	  sf_sort = ip->u.s->sort;
	  sf_name = ip->u.s->name;
	}
      else if (ip->type == 'f')
	{
	  oid = ip->u.f->oid;
	  sf_sort = ip->u.f->sort;
	  sf_name = ip->u.f->name;
	}
    }

  const char *lrefmarker = "";
  if (lp && lp->type == sl_ll_lref)
    lrefmarker = "►";
  
  fprintf(f, "%s%s\t%s\t%d\t%d\t%s\t%s\t%s\t%s", lrefmarker, name, oid,
	  tp->s, sf_sort, sf_name, code, ucun, nonull(note));

  if (lp)
    fprintf(f, "\t%s\t%s\t%s\t%s\t%s", nonull(lp->feat), nonull(lp->sname),
	    nonull(lp->ucun), nonull(lp->imagefile), nonull(lp->values));
  else
    fputs("\t\t\t\t", f);

  fputc('\n', f);

#undef nonull
}

/* Dump either the names in @listdef tags or the numbers defined in
   all the listdefs */
void
sx_list_dump(FILE *f, struct sl_signlist *sl)
{
  extern int list_names_mode, list_warnings;
  const char **n = NULL;
  int nn;
  n = hash_keys2(sl->listdefs, &nn);
  qsort(n, nn, sizeof(const char *), cmpstringp);
  int i;
  if (!list_names_mode)
    fputs("name"
	  "\toid"
	  "\tlsort"
	  "\tsfsort"
	  "\tsfname"
	  "\tucode"
	  "\tucun"
	  "\tnote"
	  "\tlfeat"
	  "\tlsname"
	  "\tlucun"
	  "\tlimage"
	  "\tlvalues"
	  "\n",
	  f);
  for (i = 0; n[i]; ++i)
    {
      if (list_names_mode)
	fprintf(stdout, "%s\n", n[i]);
      else
	{
	  struct sl_listdef *ldp = hash_find(sl->listdefs, (uccp)n[i]);
	  /* sort the list's entries if necessary */
	  if (!ldp->sorted++)
	    sx_listdefs_sort(ldp);
	  int j;
	  for (j = 0; ldp->names[j]; ++j)
	    {
	      struct sl_list *lp = hash_find(sl->hlentry, (uccp)ldp->names[j]);
	      if (lp)
		{
		  struct sl_inst *ip;
		  unsigned const char *note = (uccp)"";
		  if (lp->insts)
		    {
		      for (ip = list_first(lp->insts); ip; ip = list_next(lp->insts))
			{
			  if (lp->type == sl_ll_lref)
			    mesg_verr(&lp->inst->mloc, "strange: @lref has instances more than just its own (this can't happen)");
			  else if (lp->type != sl_ll_list)
			    mesg_verr(&ip->mloc, "untyped @list or @lref");

			  struct sl_unicode *u = (ip->type == 's' ? &ip->u.s->U : &ip->u.f->sign->U);
			  if (!u)
			    u = &ip->u.f->U;
			  struct sl_token *tp = hash_find(sl->htoken, (uccp)ldp->names[j]);
			  sx_list_row(f, sl, (uccp)ldp->names[j], ip, tp, u, lp, NULL);
			}
		    }
		  else if (lp->inst)
		    {
		      if (lp->type == sl_ll_list)
			mesg_verr(&lp->inst->mloc, "strange: @list has no instances (this can't happen)");
		      else if (lp->type == sl_ll_lref)
			{
			  if (lp->inst->notes)
			    {
			      note = (uccp)((struct sl_note*)list_first(lp->inst->notes))->txt;
			      if (note && *note)
				{
				  if ('-' == *note && '\0' == note[1])
				    note = (uccp)"";
				}
			      else
				mesg_verr(&lp->inst->mloc, "strange: @lref @note is NULL");
			    }
			  else if (!lp->sname && !lp->ucun && !lp->imagefile && !lp->values)
			    mesg_verr(&lp->inst->mloc, "@lref requires @note (use '@note -' to suppress this message)");
			}
		      else
			mesg_verr(&ip->mloc, "untyped @list or @lref");
		      struct sl_token *tp = hash_find(sl->htoken, (uccp)ldp->names[j]);
		      sx_list_row(f, sl, (uccp)ldp->names[j], ip, tp, NULL, lp, note);
		      /*fprintf(f, "%s\t\t%d\t0\t\t\t\t\t%s\n", ldp->names[j], tp->s, note);*/
		    }
		}
	      else
		{
		  if (list_warnings)
		    mesg_verr(&sl->mloc, "list entry %s missing (no @list or @lref)", ldp->names[j]);
		  struct sl_token *tp = hash_find(sl->htoken, (uccp)ldp->names[j]);
		  sx_list_row(f, sl, (uccp)ldp->names[j], NULL, tp, NULL, lp, NULL);
		  /*fprintf(f, "%s\t\t%d\t0\t\t\t\t\t[Omitted]\n", ldp->names[j], tp->s);*/
		}
	    }
	}
    }
}
