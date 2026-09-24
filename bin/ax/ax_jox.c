#include <oraccsys.h>
#include <xmlify.h>
#include <pool.h>
#include <tree.h>
#include <xnn.h>
#include <ns-xtf.h>
#include <joxer.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <gt.h>
#include <asl.h>
#include "ax.h"

const char *static_trafile = "ax.xtr";
const char *static_xmlfile = "ax.xml";

extern Mloc *xo_loc;
extern int trace_mode;
const char *xfn = NULL;
const char *tfn = NULL;
extern const char *file;
static void ax_jox_lines(Group *gp);
static void ax_jox_node(Node *np);

static FILE *tra_fp, *xtf_fp, *atf_pi_fp;

static int no_pi = 0;

static void
atf_file_pi(const char *file)
{
  if (!no_pi)
    fprintf(atf_pi_fp,"<?atf-file %s?>",file);
}

static void
atf_line_pi(Node *n)
{
  if (!no_pi)
    {
      fputs("<?atf-line ",atf_pi_fp);
      fprintf(atf_pi_fp,"%d",n->mloc->line);
      fputs("?>",atf_pi_fp);
#if 0
      if (pretty)
	fputc('\n',f_xml);
#endif
    }
}

int
ax_gdl_jox(Node *np, void *vp)
{
  grx_jox_gdl(np, np->user);
  return 0;
}

static void
ax_htm_node(Node *np, void *ignored)
{
  if ('-' == *np->name)
    joxer_ch(np->mloc, np->text);
  else
    {
      Ratts *r = NULL;
      const char **p = NULL;
      joxer_ea(np->mloc, np->name, (r = rnvval_aa_ccpp((p = ax_jox_props(np->props)))));
      if (p)
	free(p);
      if (r)
	{
	  free(r->atts);
	  free(r->qatts);
	  free(r);
	}
      Node *k;
      for (k = np->kids; k; k = k->next)
	ax_htm_node(k, ignored);
      joxer_ee(np->mloc, np->name);
    }
}

int
ax_htm_jox(Node *np, void *fp)
{
  ax_htm_node(np, fp);

  return 0;
}

void
ax_jox_tra(void)
{
  if (!tra_fp)
    tra_fp = xfopen(trafile, "w");
  if (tra_fp)
    {
      jox_xml_output(tra_fp);
      atf_pi_fp = tra_fp;
      atf_file_pi(file);
      joxer_init(&xtf_data, "xtf", val_flag, tra_fp, NULL);
      ax_jox_node(curr_trans->tree->root);
      joxer_term(tra_fp, NULL);
      xfclose(trafile, tra_fp);
      tra_fp = NULL;
    }
  curr_trans = NULL;
}

void
ax_jox(Tree *tp)
{
  if (!xtf_fp && !xfn)
    {
      xfn = static_xmlfile;
      trafile = (char*)static_trafile;
    }

  FILE *xfp = NULL;

  xo_loc = memo_auto(sizeof(Mloc));
  xo_loc->file = file = tp->root->mloc->file;
  xo_loc->line = 1;

  if (!xtf_fp)
    {
      if (xml_to_stdout)
	xfp = stdout;
      else if (!(xfp = fopen(xfn, "w")))
	{
	  fprintf(stderr, "ax: failed to open XML output %s. Stop.\n", xfn);
	  exit(1);
	}
      jox_xml_output(xfp);
      atf_pi_fp = xfp;
      atf_file_pi(file);
      joxer_init(&xtf_data, "xtf", val_flag, xfp, NULL);
      ax_jox_node(tp->root);
      joxer_term(xfp,NULL);
      fclose(xfp);
      if (curr_trans && curr_trans->tree->root->kids)
	ax_jox_tra();
    }
  else
    {
      if (tra_fp)
	{
	  if (curr_trans && curr_trans->tree->root->kids)
	    {
	      ax_jox_tra();
	    }
	  else
	    {
	      xfclose(trafile, tra_fp);
	      tra_fp = NULL;
	      unlink(trafile);
	    }
	}
      else if (curr_trans)
	tree_root_append(tp, curr_trans->tree->root);

      if (xtf_fp)
	{
	  jox_xml_output(xtf_fp);
	  atf_pi_fp = xtf_fp;
	  atf_file_pi(file);
	  joxer_init(&xtf_data, "xtf", val_flag, xtf_fp, NULL);
	  ax_jox_node(tp->root);
	  joxer_term(xtf_fp, NULL);
	  xfclose(xtffile, xtf_fp);
	  xtf_fp = NULL;
	}
    }
}

int
ax_outputs(char *xtf, char *tra)
{
  if (xtf && !(xtf_fp = xfopen(xtf, "w")))
    return 1;

  if (tra && !(tra_fp = xfopen(tra, "w")))
    {
      if (xtf_fp)
	xfclose(xtf, xtf_fp);
      return 1;
    }

  return 0;
}

const char **
ax_jox_props(Prop *p)
{
  const char **ap = prop_ccpp(p, -1, PG_XML);
  const char **gp = prop_ccpp(p, GP_ATTRIBUTE, PG_GDL_INFO);
  if (ap && gp)
    {
      int n_ap = 0, n_gp = 0;
      for (n_ap = 0; ap[n_ap]; ++n_ap)
	;
      for (n_gp = 0; gp[n_ap]; ++n_gp)
	;
      const char **newp = malloc(n_ap+n_gp+1 * sizeof(const char *));
      memcpy(newp, ap, n_ap*sizeof(const char *));
      memcpy(newp+n_ap, gp, n_gp*sizeof(const char *));
      newp[n_ap+n_gp] = NULL;
      free(ap);
      free(gp);
      return newp;
    }
  else if (ap)
    return ap;
  else
    return gp;
}

/* This routine is a dispatcher for handling the current node.  If
 * there is an axjoxfunc, control is delegated to that function, which
 * must then also handle child nodes or return 1 to indicate child
 * node handling should be done by ax_jox_node; if the Node ns ==
 * NS_GDL, grx_jox_gdl is called via the wrapper ax_gdl_jox, which
 * also handles child nodes.  Otherwise, processing is handled within
 * the routine.
 */
static void
ax_jox_node(Node *np)
{
  Node *npp;
  const char *nodename = np->name;
  struct axjoxfnc *ap = NULL;

  if (np->name && strcmp(np->name, "-") && np->mloc && strcmp(np->name, "lg"))
    atf_line_pi(np);

  if (trace_mode)
    fprintf(stderr, "ax_jox_node: nodename = %s\n", nodename);
  
  if (np->user)
    ap = axjoxfnc(nodename,strlen(nodename));

  if (!ap && NS_GDL == np->ns)
    ap = axjoxfnc("_gdl_", 5);

  else if (!ap && NS_HTM == np->ns)
    ap = axjoxfnc("_htm_", 5);

  Ratts *r = NULL;
  const char **p = NULL;
  if (!ap || ap->wrapper || np->utype == N_U_XLEM) /* N_U_XLEM is a g:w with LEM info */
    {
      if ('-' != *nodename)
	joxer_ea(np->mloc, nodename, (r = rnvval_aa_ccpp((p = ax_jox_props(np->props)))));
      if (p)
	free(p);
      if (r)
	{
	  free(r->atts);
	  free(r->qatts);
	  free(r);
	}
    }
  if ((ap || np->user) && np->utype != N_U_XLEM)
    {
      if (ap)
	{
	  if (ap->func(np, np->user) > 0)
	    /* Descend recursively into child nodes on a return value != 0*/
	    for (npp = np->kids; npp; npp = npp->next)
	      ax_jox_node(npp);
	}
      else
	{
	  if (blocktok(nodename, strlen(nodename)))
	    {
	      ax_jox_block(np, np->user);
	      /* Descend recursively into child nodes */
	      for (npp = np->kids; npp; npp = npp->next)
		ax_jox_node(npp);
	    }
	  else
	    fprintf(stderr, "ax_jox_node: internal error: no handler for tag '%s'\n",
		    nodename);
	}
    }
#if 0
  /* This is handled by ax_htm_jox */
  else if (N_U_SCANSEG == np->utype && np->kids)
    {
      for (npp = np->kids; npp; npp = npp->next)
	ax_jox_node(npp);
    }
#endif
  else if (np->text)
    {
      joxer_ch(np->mloc, np->text);
    }
  else
    {
      /* Descend recursively into child nodes */
      for (npp = np->kids; npp; npp = npp->next)
	ax_jox_node(npp);
    }

  if ((!ap || ap->wrapper || np->utype == N_U_XLEM) && '-' != *nodename)
    joxer_ee(np->mloc, nodename);
}

/* handler functions for np->user and GDL */
int
ax_jox_bib(Node *np, Bib *p)
{
  joxer_ch(np->mloc, (ccp)p->text);
  return 0;
}

void
ax_jox_block(Node *np, Block *p)
{
  /* none of the block functions needs this yet */
}

void
ax_jox_lang(Node *np, ATF *a)
{
  atf_line_pi(np);
  char buf[(a->altlang?strlen(a->altlang)+3:0)+strlen(a->lang)+2];
  if (a->altlang)
    sprintf(buf, "lang %s _%s_", a->lang, a->altlang);
  else
    sprintf(buf, "lang %s", a->lang);
  joxer_ch(np->mloc, buf);
}

int
ax_jox_lg(Node *np, Group *gp)
{
  if (gp->line_lines > 1)
    {
      joxer_ea(np->mloc, "lg", NULL);
      ax_jox_lines(gp);
      joxer_ee(np->mloc, "lg");
    }
  else
    ax_jox_lines(gp);
  return 0;
}

static void
ax_jox_lines(Group *gp)
{
  int n;
  for (n = 0; n < gp->nlines; ++n)
    {
      Node *np = gp->lines[n]->np;
      if (np->kids)
	{
	  ax_jox_node(np);
	}
      else
	{
	  if (n)
	    atf_line_pi(np);
	  const char **p = NULL;
	  Ratts *r = NULL;
	  joxer_ea(np->mloc, np->name, (r = rnvval_aa_ccpp((p = ax_jox_props(np->props)))));
	  if (p)
	    free(p);
	  if (r)
	    {
	      free(r->atts);
	      free(r->qatts);
	      free(r);
	    }
	  if (np->text)
	    joxer_ch(np->mloc, np->text);
	  joxer_ee(np->mloc, np->name);
	}
    }
}

int
ax_jox_note(Node *np, Note *p)
{
  joxer_ch(np->mloc, (ccp)p->text);
  return 0;
}

/* This is to handle atf: lang -- the void ptr is ATF * */
int
ax_jox_protocol(Node *np, void *p)
{
  atf_line_pi(np);
  Prop*ptype = prop_find_kv(np->props, "type", NULL);
  if (ptype)
    {
      if (!strcmp(ptype->u.k->v, "bib"))
	ax_jox_bib(np, p);
      else if (strcmp(ptype->u.k->v, "after"))
	ax_jox_lang(np, p);
    }
  return 0;
}

