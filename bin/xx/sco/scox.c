#include <oraccsys.h>
#include <xml.h>

const char *lkb_project;
Hash *P, *PQ, *Q;
Hash *exemplar_lists, *exemplar_links, *Q_exemplars;
Hash *parallel_lists, *source_lists, *Q_parallels;
Hash *sp;
int verbose;
List *scolist;
/*Pool *p;*/

void **
ptrpair(void *vp1, void *vp2)
{
  void **p = malloc(2*sizeof(void *));
  p[0] = vp1;
  p[1] = vp2;
  return p;
}

/* Return the PQX part of a QID only if the QID project == the linkbase project */
const char *
pqx_of(const char *q)
{
  if (!strncmp(q, lkb_project, strlen(lkb_project))
      && ':' == q[strlen(lkb_project)]
      && ('Q' == q[strlen(lkb_project)+1]
	  || 'X' == q[strlen(lkb_project)+1])
	  )
    return q+strlen(lkb_project)+1;
  else
    return NULL;
}

char *
qid(const char *r, const char *l)
{
  char buf[strlen(r)+strlen(l)];
  strcpy(buf, r);
  strcat(buf, strchr(l, '.'));
  return memo_dup(buf);
}

void
hash_hash_add(Hash *h, const char *k, const char *kk)
{
  Hash *hh = hash_find(h, (uccp)k);
  if (!hh)
    hash_add(h, (uccp)k, (hh = hash_create(128)));
  hash_add(hh, (uccp)kk, "");
}

void
hash_list_add(Hash *h, const char *k, void *vp)
{
  List *lp = hash_find(h, (uccp)k);
  if (!lp)
    hash_add(h, (uccp)k, lp = list_create(LIST_SINGLE));
  list_add(lp, vp);
}

void
create_rel_lists(List *tl)
{
  Node *np;
  for (np = list_first(tl); np; np = list_next(tl))
    {
      if ('l' == *np->name)
	{
	  const char *rel = xlt_att(np, "rel");
	  const char *fref = xlt_att(np->kids, "ref");
	  const char *tref = xlt_att(np->kids->next, "ref");
	  const char *fline = xlt_att(np->kids, "line");
	  const char *tline = xlt_att(np->kids->next, "line");
	  hash_add(PQ, (uccp)fref, "");
	  hash_add(PQ, (uccp)tref, "");
	  hash_add(P, (uccp)fref, "");
	  const char *tpqx = pqx_of(tref);
	  if (tpqx)
	    {
	      hash_add(Q, (uccp)tpqx, "");
	      switch (*rel)
		{
		case 'g':
		  {
		    char *qpqx = qid(tref, tline);
		    hash_hash_add(Q_exemplars, tpqx, qpqx);
		    hash_list_add(exemplar_lists, qpqx, (void*)qid(fref,fline));
		    hash_list_add(exemplar_links, fref, ptrpair((void*)fline, (void*)tline));
		  }
		  break;
		case 'p':
		  hash_list_add(Q_parallels, tpqx, (void*)qid(tref, tline));
		  hash_list_add(parallel_lists, tline, (void*)qid(fref,fline));
		  hash_list_add(parallel_lists, fline, (void*)tline);
		  break;
		case 'c':
		  hash_list_add(source_lists, fline, (void*)tline);
		  break;
		default:
		  fprintf(stderr, "scox: corrupt <link> in 01bld/linkbase.xml. Stop.");
		  exit(1);
		}
	    }
	}
      else
	{
	  const char *t = xlt_att(np, "type");
	  switch (*t)
	    {
	    case 's':
	      {
		void **pp = ptrpair(np, NULL);
		hash_add(sp, (uccp)xlt_att(np, "ref"), pp);
	      }
	      break;
	    case 'p':
	      {
		void **pp = ptrpair(NULL, np);
		hash_add(sp, (uccp)xlt_att(np, "ref"), pp);
	      }
	      break;
	    default:
	      fprintf(stderr, "scox: corrupt <ref> in 01bld/linkbase.xml. Stop.");
	      exit(1);
	    }
	}
    }
}

void
create_one_score(const char *q)
{
  const char *qfn = expand(lkb_project, q, "sci");
  if (verbose)
    fprintf(stderr, "scox: qfn: %s\n", qfn);
  FILE *fp = xfopen(qfn, "w");
  if (fp)
    {
      fputs("<sci xmlns:xi=\"http://www.w3.org/2001/XInclude\">", fp);
      fputs("<ees>", fp);
      int i;
      Hash *qh = hash_find(Q_exemplars, (uccp)q);
      if (qh)
	{
	  const char **qek = hash_keys(qh);
	  for (i = 0; qek[i]; ++i)
	    {
	      const char *ql = qek[i];
	      fprintf(fp, "<ee xml:id=\"%s.e\">", strchr(ql,':')+1);
	      List *elines = hash_find(exemplar_lists, (uccp)ql);
	      const char *el;
	      for (el = list_first(elines); el; el = list_next(elines))
		{
		  char *xtf = expand(NULL, el, "xtf");
		  fprintf(fp,
			  "<e><xi:include href=\"%s\" xpointer=\"xpointer(id('%s'))\"/></e>",
			  xtf, strchr(el,':')+1);
		}
	      fputs("</ee>", fp);
	    }
	}
      fputs("</ees>", fp);
      fputs("<pblocks>", fp);
      const char **pk = hash_keys(parallel_lists);
      fputs("</pblocks>", fp);
      fputs("</sci>", fp);
    }
  else if (verbose)
    fprintf(stderr, "scox: skipping %s\n", q);
}

void
create_scores(void)
{
  const char **qk = hash_keys(Q);
  int i;
  for (i = 0; qk[i]; ++i)
    create_one_score(qk[i]);
}

void
scox_init(Tree *tp)
{
  if (strcmp(tp->root->kids->name, "project"))
    {
      fprintf(stderr, "scox: 01bld/linkbase.xml does not have a <project> child. Stop.\n");
      exit(1);
    }
  else
    {
      lkb_project = xlt_att(tp->root->kids, "n");
      P = hash_create(128);
      PQ = hash_create(128);
      Q = hash_create(128);
      Q_exemplars = hash_create(128);
      Q_parallels = hash_create(128);
      exemplar_lists = hash_create(128);
      exemplar_links = hash_create(128);
      parallel_lists = hash_create(128);
      source_lists = hash_create(128);
      sp = hash_create(128);
      /*p = pool_init();*/
    }
}

int
main(int argc, char *const*argv)
{
  Tree *tp = xml_load_tree("01bld/linkbase.xml", 0);
  if (!tp)
    {
      fprintf(stderr, "scox: failed to load 01bld/linkbase.xml. Stop.\n");
      exit(1);
    }
  scox_init(tp);
  Hash *tags = hash_create(1);
  hash_add(tags, (uccp)"link", "");
  hash_add(tags, (uccp)"refs", "");  
  List *tl = xlt_tags_hash(tp->root, tags);
  create_rel_lists(tl);
  scolist = list_create(LIST_SINGLE);
  create_scores();
}
