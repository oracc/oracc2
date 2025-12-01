#include <oraccsys.h>
#include <runexpat.h>
#include "bx.h"
#include "bib.h"

static Bibicf *bx_icf_data(Bx *bp, Bibentry *ep);
static void bx_run_icf(Bx *bp, const char *fname);

static int span_pending;

void
bx_icf_pre(Bx *bp)
{
  if (bp->keys_from_bib)
    {
      bx_key_pre(bp);
      bx_key_run(bp);
    }

  /* call bx_ref which will call bx_cit so the two modes will set
     things up for ICF between them */
  bx_ref_pre(bp);

  /* bx_ref_pre calls CIT mode to load keys */
  if (bp->keys_cit->key_count)
    bx_ref_run(bp);
}

void
bx_icf_run(Bx *bp)
{
  if (bp->ents)
    {
      /* Build ICF output strings for every entry in bib */
      bp->m_bibicf = memo_init(sizeof(Bibicf), 256);
      bp->hicf = hash_create(1024);
      int i;
      for (i = 0; bp->ents[i]; ++i)
	hash_add(bp->hicf, (uccp)bp->ents[i]->bkey,
		 (bp->ents[i]->icf = bx_icf_data(bp, bp->ents[i])));
    }
}

void
bx_icf_out(Bx *bp)
{
  bp->outfp = stdout; /* place-holder for better things */
  int i;
  /* icf always runs over same inputs as cit */
  for (i = 0; bp->files_cit[i]; ++i)
    bx_run_icf(bp, bp->files_cit[i]);

  /* Do this after the Bibicf building so we can include it in bib
     output if appropriate */
  if (!bp->icfonly)
    bx_ref_out(bp);
}

static const char *
bx_icf_str(Bx *bp, Bibentry *ep)
{
  const char *name = "", *init = "", *etal = "", *eds = "", *year = "", *comma = "";
  if (ep->fields[f_author])
    {
      name = ep->nm_author->names[0]->last;
      if (ep->nm_author->names[0]->sames)
	{
	  init = ep->nm_author->names[0]->init;
	  comma = ", ";
	}
      if (ep->nm_author->names[1])
	etal = " et al.";
    }
  else
    {
      name = ep->nm_editor->names[0]->last;
      if (ep->nm_editor->names[0]->sames)
	init = ep->nm_editor->names[0]->init;
      if (ep->nm_editor->names[1])
	{
	  etal = " et al.";
	  eds = " (eds)";
	}
      else
	eds = " (ed)";
    }
  
  year = ep->fields[f_year]->data;
  
  int len = strlen(name)+strlen(comma)+strlen(init)+strlen(etal)+strlen(eds)+strlen(year)+2;
  char buf[len];
  sprintf(buf, "%s%s%s%s%s %s", name, comma, init, etal, eds, year);
  return (ccp)pool_copy((uccp)buf, bp->p);
}
 
static Bibicf *
bx_icf_data(Bx *bp, Bibentry *ep)
{
  Bibicf *bip = memo_new(bp->m_bibicf);
  bip->str = bx_icf_str(bp, ep);
  bip->ep = ep;
  return bip;
}

static void
bx_icf_cite_one(Bx *bp, const char *r)
{
  Bibicf *ip = hash_find(bp->hicf, (uccp)r);
  if (ip)
    fprintf(bp->outfp, "%s", ip->str);
  else
    fprintf(stderr, "bx: failed to find cite string for key `%s'\n", r);
}

static void
bx_icf_cite(Bx *bp, const char *ref)
{
  bx_icf_cite_one(bp, ref);
}

/* This printText implements the same escaping as used by oracc2's
   xmlify library routine */
void
printText(const char *s, FILE *frag_fp)
{
  while (*s)
    {
      if (*s == '<')
	fputs("&lt;",frag_fp);
      else if (*s == '>')
	fputs("&gt;",frag_fp);
      else if (*s == '&')
	fputs("&amp;",frag_fp);
#if 0
      else if (*s == '\'')
	fputs("&apos;",frag_fp);
#endif
      else if (*s == '"')
	fputs("&quot;",frag_fp);
      else
	fputc(*s,frag_fp);
      ++s;
    }
}

void
printStart(FILE *fp, const char *name, const char **atts)
{
  const char **ap = atts;
  printText((const char*)charData_retrieve(), fp);
  fprintf(fp, "<%s", name);
  if (atts)
    {
      for (ap = atts; ap[0]; )
	{
	  fprintf(fp, " %s=\"",*ap++);
	  printText(*ap++, fp);
	  fputc('"', fp);
	}
    }
  fputc('>', fp);
}

void
printEnd(FILE *fp, const char *name)
{
  printText((const char *)charData_retrieve(), fp);
  fprintf(fp, "</%s>", name);
}

#define ubp	((Bx*)userData)

void
icf_sH(void *userData, const char *name, const char **atts)
{
  static int hmode = 0;
  if (!strcmp(name, "b:cite"))
    {
      /* If we are emitting HTML output we probably want to put the
	 cite in span with a JS link to the full bib item */
      if (hmode)
	{
	  printStart(ubp->outfp, "span", atts);
	  span_pending = 1;
	}
      else
	printStart(ubp->outfp, name, atts);
      bx_icf_cite(userData, findAttr(atts, "key"));
    }
  else
    {
      if (span_pending)
	++span_pending;
      printStart(ubp->outfp, name, atts);
    }
}

void
icf_eH(void *userData, const char *name)
{
  if (span_pending)
    {
      --span_pending;
      if (!span_pending)
	name = "span";
    }
  else
    printEnd(ubp->outfp, name);
}

static void
bx_run_icf(Bx *bp, const char *fname)
{
  char const *fnlist[2];
  fnlist[0] = fname;
  fnlist[1] = NULL;
  runexpatNSuD(i_list, fnlist, icf_sH, icf_eH, NULL, bp);
}

