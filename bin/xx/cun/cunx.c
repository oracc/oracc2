#include <oraccsys.h>
#include <xmlify.h>
#include <setjmp.h>
#include "runexpat.h"
#include "cun.h"

/* Structure for damaged tokens */
#define CT_NOT 0
#define CT_WSP 0x01
#define CT_ELL 0x02
#define CT_XXX 0x04
#define CT_GRP 0x08

List *cqueue = NULL;

int in_word; /* incremented by grapheme-start */

struct d
{
  int type;
  const char *name;
  const char *oid;
  const char *utf8;
  const char *form;
  const char *xid; /* xml:id */
  Hash *atts;
  int broken; /* used for spaces--if it is inside a broken sequence this is 1 */
};
Memo *mem_d;
Pool *p;

extern int file_args(const char *htmldir, const char *qpqx, const char *inext,
		     const char *outdir, const char *outext, const char *trans,
		     char **inp, char **outp, char **hdir);

/* The default is to generate outputs into the P4 cache; if -b is
   specified outputs are generated into the PQX-dir where the xtf
   lives */
int buildmode = 0;
int force = 0; /* default is not to remake if the output already exists */
int no_output = 0;
int verbose = 0;
int weboutput = 0;

const char *invoke;
const char *infile;
const char *outfile;
const char *p4htmld;
const char *period;
const char *project;
const char *qfile;
const char *translation;

char *inpath, *outpath, *htmlpath;

FILE *outfp = NULL;

jmp_buf done;

extern int options(int, char*const*,const char*);
extern int optind;

int hc_pending = 0; /* set when a grapheme has a close-half-square on it */
int in_break = 0, in_missing = 0;
int in_c = 0, in_l = 0, in_n = 0, in_q = 0;
int last_was_ellipsis = 0;
int word_count = 0;
int ws_pending = 0;

typedef struct Cun_class
{
  const char *fnt;
  const char *mag;
  const char *scr;
} Cun_class;

Cun_class cun_defaults = { .fnt="noto" , .mag="100", .scr="middle" };
Cun_class *curr_cp = &cun_defaults;

static const char *
cc_skip_prefix(const char *c)
{
  const char *ofs = "ofs-";
  if ('-' == c[0] && '-' == c[1])
    c += 2;
  if (!strncmp(c, ofs, strlen(ofs)))
    c += strlen(ofs);
  return c;
}

static Cun_class *
cun_class(const char **atts, Cun_class *curr_cp)
{
  static Cun_class cp;
  const char *fnt = findAttr(atts, "fnt");
  const char *mag = findAttr(atts, "mag");
  const char *scr = findAttr(atts, "scr");

  if (*fnt)
    fnt = cc_skip_prefix(fnt);
  if (*mag)
    mag = cc_skip_prefix(mag);
  if (*scr)
    scr = cc_skip_prefix(scr);
  
  cp.fnt = *fnt ? fnt : curr_cp->fnt;
  cp.mag = *mag ? mag : curr_cp->mag;
  cp.scr = *scr ? scr : curr_cp->scr;
  return &cp;
}

void
cfy_head(FILE *fp, const char *n, Cun_class *cp)
{
  if (!weboutput) /* paradoxically, weboutput skips the html head/body because those are provided by P4 */
    {
      const char *cfyfam = cfy_fam(project);
      fprintf(fp,
	      "<html >"
	      "<head><meta charset=\"utf-8\"/>"
	      "<title>Cuneified %s</title>"
	      "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/fonts.css\"/>"
	      "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p4-cuneify.css\"/>"
	      "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\"/>"
	      "<script type=\"text/javascript\" src=\"/js/p4.js\">&#160;</script>"
	      "</head><body onload=\"p4Onload()\">", n, cfyfam);
    }
  fprintf(fp,
	  "<div id=\"p4Cuneify\" "
	  "data-cfy-fnt=\"%s\" data-cfy-mag=\"%s\" data-cfy-scr=\"%s\" data-proj=\"%s\">",
	  cp->fnt, cp->mag, cp->scr, project);
  fprintf(outfp, "<h1 class=\"p3h2 border-top heading\"><span class=\"cfy-generic\">Cuneified </span><span class=\"cfy-specific\">%s</span></h1><table class=\"cfy-table\">", n);
}

#if 0
void
cfy_space(void)
{
  fprintf(outfp, "<span class=\"cfy-ws\"> </span>");  
}
#endif

void
cfy_ellipsis(void)
{
  fprintf(outfp, "<span class=\"roman\">%s. . .</span>", last_was_ellipsis ? " " : "");
}

void
cfy_x(void)
{
  fprintf(outfp, "<span class=\"roman gray\">×</span>");
}

void
cfy_cun(const char *name, const char *xid, const char *oid, const char *utf8, const char *form)
{
#if 1
  const char *miss = (in_missing ? " missing" : "");
  const char *space = "";

  if (in_word > 1)
    space = " cfy-gsp";
  else if (ws_pending)
    {
      space = " cfy-wsp";
      ws_pending = 0;
    }
  
  if (oid)
    fprintf(outfp,
	    "<span id=\"c.%s\" title=\"%s\" "
	    "data-oid=\"%s\" onclick=\"cfySL(event)\" "
	    "class=\"cfy-fam cfy-def%s%s\">%s</span>",
	    xid, form, oid, miss, space, utf8);
  else
    fprintf(outfp, "<span id=\"c.%s\" title=\"%s\" class=\"cfy-fam cfy-def%s%s\">%s</span>",
	    xid, form, miss, space, utf8);
#else
  const char *miss = (in_missing ? " missing" : "");
  if (oid)
    fprintf(outfp, "<span id=\"c.%s\" class=\"cfy-cun%s\"><a href=\"/osl/signlist/%s\" title=\"%s\">%s</a></span>",
	    xid, miss, oid, form, utf8);
  else
    fprintf(outfp, "<span id=\"c.%s\" title=\"%s\" class=\"cfy-cun%s\">%s</span>", xid, form, miss, utf8);
#endif
}

void
cfy_foot(FILE *fp)
{
  fprintf(fp, "</table></div>");
  if (!weboutput)
    fprintf(fp, "</body></html>");
}

int
printable(const char *name, const char **atts, const char **utf8p)
{
  const char *utf8 = findAttr(atts, "g:utf8");
  if (*utf8)
    {
      if (utf8p)
	*utf8p = utf8;
      return *utf8;
    }
  if (!strcmp(name, "g:x"))
    return 1;
  return 0;
}

int
breakage(const char *name, const char **atts)
{
  const char *utf8 = findAttr(atts, "g:utf8");
  const char *b = findAttr(atts, "g:break");
  if (!strcmp(b, "missing"))
    return 1;
  else if (!strcmp(b, "damaged") || 'x' == *utf8 || 'X' == *utf8)
    return 1;
  return 0;
}

Hash *
hashatts(const char **atts)
{
  Hash *h = hash_create(7);
  Pool *pp = pool_init();
  hash_add(h, (ucp)"#p", pp);
  int i;
  for (i = 0; atts[i]; i += 2)
    hash_add(h, pool_copy((ucp)atts[i],pp), pool_copy((ucp)atts[i+1],pp));
  return h;
}

static void
d_free(struct d *dp)
{
  Pool *pp = hash_find(dp->atts, (uccp)"#p");
  pool_term(pp);
  hash_free(dp->atts, NULL);
}

static int
d_type(const char *name, const char **atts, const char *utf8)
{
  const char *gt = NULL;
  if (!atts)
    return CT_WSP;
  else if (utf8 && ('x' == *utf8 || 'X' == *utf8))
    return CT_XXX;
  else if ((gt=findAttr(atts,"g:type")) && !strcmp(gt,"ellipsis"))
    return CT_ELL;
  else if (strlen(name)==3 && strspn(name+2,"svcnq"))
    return CT_GRP;
  else
    return CT_NOT;
}

void
enqueue(const char *name, const char **atts, const char *oid, const char *utf8, const char *form)
{
  if (!cqueue)
    cqueue = list_create(LIST_DOUBLE);
  struct d *dp = memo_new(mem_d);
  dp->type = d_type(name, atts, utf8);
  dp->name = (ccp)pool_copy((ucp)name, p);
  dp->oid = oid;
  dp->utf8 = utf8;
  dp->form = form;
  if (atts)
    {
      dp->xid = (ccp)pool_copy((ucp)get_xml_id(atts), p);
      dp->atts = hashatts(atts);
    }
  list_add(cqueue, dp);
}

void
damagedws(void)
{
  ((struct d*)list_last(cqueue))->broken = 1;
}

void
dequeue(void)
{
  if (!list_len(cqueue))
    return;
  
  struct d *dp;
  int span_closed = 0;
  fprintf(outfp, "<span class=\"broken\">");
  for (dp = list_first(cqueue); dp; dp = list_next(cqueue))
    {
#if 0
      if (ws_pending)
	{
	  cfy_space();
	  ws_pending = 0;
	}
#endif
      const char *go = (ccp)hash_find(dp->atts, (ucp)"g:o");
      if (go && strchr(go, '['))
	{
	  fprintf(outfp, "<span class=\"roman\">[</span>");
	  in_missing = 1;
	}
      switch (dp->type)
	{
	case CT_WSP:
	  if (!dp->broken)
	    {
	      fputs("</span>", outfp);
	      span_closed = 1;
	    }
	  ws_pending = 1;
	  break;
	case CT_ELL:
	  cfy_ellipsis();
	  break;
	case CT_XXX:
	  cfy_x();
	  break;
	case CT_GRP:
	  cfy_cun(dp->name, dp->xid, dp->oid, dp->utf8, dp->form);
	  break;
	default:
	  fprintf(stderr, "cunx: unknown token in cuneify queue\n");
	  break;
	}
      const char *gc = hash_find(dp->atts, (ucp)"g:c");
      if (gc && strchr(gc, ']'))
	{
	  fprintf(outfp, "<span class=\"roman\">]</span>");
	  in_missing = 0;
	}
      d_free(dp);
    }
  if (!span_closed)
    fputs("</span>", outfp);
  list_free(cqueue, NULL);
  cqueue = NULL;
}

void
ei_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "xcl"))
    longjmp(done, 0);

  if (!strcmp(name, "transliteration") || !strcmp(name, "composite"))
    {
      const char *xn = (ccp)xmlify((uccp)findAttr(atts, "n"));
      Cun_class *cp = cun_class(atts, curr_cp);
      *curr_cp = *cp;
      cfy_head(outfp, xn, cp);
    }
  else
    {
      if (in_l && !in_c && !in_n && !in_q)
	{
	  const char *utf8 = NULL;
	  if (printable(name, atts, &utf8))
	    {
#if 0
	      /* ws_pending is only set when we are not in breakage so
		 if we find something printable it's always right to
		 emit the space */
	      if (ws_pending)
		{
		  cfy_space();
		  ws_pending = 0;
		}
#endif

	      ++in_word;
	      
	      const char *form = findAttr(atts, "form");
	      if (!form || !*form)
		{
		  form = findAttr(atts, "key");
		  if (form && *form)
		    form = strrchr(form,'.')+1;
		}
	      if (form && *form)
		form = (ccp)pool_copy((ucp)form, p);
	      const char *oid = findAttr(atts, "spoid");
	      if (!oid || !*oid)
		oid = findAttr(atts, "oid");
	      if (*oid)
		oid = (ccp)pool_copy((ucp)oid, p);
	      else
		oid = NULL;
	      if (utf8 && *utf8)
		utf8 = (ccp)pool_copy((ucp)utf8, p);
	      if (breakage(name, atts))
		{
		  if (cqueue)
		    damagedws();
		  enqueue(name, atts, oid, utf8, form);
		}
	      else
		{
		  dequeue();
#if 0
		  /* Spaces in the queue set ws_pending rather than calling cfy_space directly */
		  if (ws_pending)
		    {
		      cfy_space();
		      ws_pending = 0;
		    }
#endif
		  cfy_cun(name, get_xml_id(atts), oid, utf8, form);
		}
	    }

	  if (!strcmp(name, "g:n"))
	    in_n = 1;
	  else if (!strcmp(name, "g:q"))
	    in_q = 1;
	  else if (!strcmp(name, "g:c"))
	    in_c = 1;
	}
      else
	{
	  if (!strcmp(name, "l"))
	    {
	      fprintf(outfp, "<tr><td class=\"cuneify-label\">%s</td><td class=\"cuneify-content\">", findAttr(atts, "label"));
	      in_word = ws_pending = last_was_ellipsis = word_count = 0;
	      in_l = 1;
	    }
	}
    }
}

void
ei_eH(void *userData, const char *name)
{
  if (!strcmp(name, "l"))
    {
      dequeue();
      fprintf(outfp, "</td></tr>");
      in_l = last_was_ellipsis = 0;
    }
  else if (!strcmp(name, "g:n"))
    in_n = 0;
  else if (!strcmp(name, "g:q"))
    in_q = 0;
  else if (!strcmp(name, "g:c"))
    in_c = 0;
  else if (!strcmp(name, "g:w"))
    {
      if (list_len(cqueue))
	enqueue(name, NULL, NULL, NULL, NULL);
      else
	{
	  ws_pending = 1;
	  in_word = 0;
	}
    }
}

static void
show_vars(void)
{
  fprintf(stderr, "cunx variables:\n");
  fprintf(stderr, "\tinput-arg: %s\n", qfile ? qfile : infile);
  fprintf(stderr, "\tinput-path: %s\n", inpath);
  fprintf(stderr, "\toutput-path: %s\n", outpath);
}

int
main(int argc, char **argv)
{
  outfp = stdout;
  options(argc, argv, "bfi:l:no:p:P:q:st:vw");

  if (period)
    {
      struct perfnt *pfp = perfnt(period, strlen(period));
      if (pfp)
	cun_defaults.fnt = pfp->fnt;
    }
  
  if (!infile && argv[optind])
    infile = argv[optind++];

  const char **f = calloc(2, sizeof(char*));
  
  if (infile || qfile)
    {
      if (weboutput)
	{
	  if (!p4htmld)
	    {
	      const char *p4cache = getenv("ORACC_P4_CACHE");
	      if (p4cache)
		{
		  p4htmld = malloc(strlen(p4cache)+5);
		  sprintf((char*)p4htmld, "%s/htm", p4cache);
		}
	      else
		p4htmld = "/home/oracc/www/p4.d/htm";
	    }
	}
      if (qfile)
	{
	  project = strdup(qfile);
	  char *colon = strchr(project, ':');
	  if (colon)
	    *colon = '\0';
	  else
	    {
	      fprintf(stderr, "cunx: must give project:PQX when using -q option. Stop.\n");
	      exit(1);
	    }
	  file_args(p4htmld, qfile, "xtf", outfile, "cfy", NULL, &inpath, &outpath, &htmlpath);
	  f[0] = inpath;
	}
      else if (infile)
	{
	  inpath = strdup(infile);
	  f[0] = inpath;
	  if (outfile)
	    outpath = strdup(outfile);
	}
    }
  
  if (verbose)
    show_vars();

  if (!no_output)
    {
      if (f[0] && !access(f[0], R_OK))
	{
	  if (outpath && strcmp(outpath, "-"))
	    {
	      if (!(outfp = fopen(outpath, "w")))
		{
		  fprintf(stderr, "cunx: unable to write to outpath %s: %s\n", outpath, strerror(errno));
		  exit(1);
		}
	    }

	  runexpat_omit_rp_wrap();

	  p = pool_init();
	  mem_d = memo_init(sizeof(struct d), 10);
  
	  if (!setjmp(done))
	    runexpat(i_list, f, ei_sH, ei_eH);
	  
	  cfy_foot(outfp);

	  pool_term(p);
	  memo_term(mem_d);
	}
      else
	fprintf(stderr, "cunx: nothing to cuneify. Stop.\n");
    }
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'b':
      buildmode = 1;
      break;
    case 'f':
      force = 1;
      break;
    case 'i':
      infile = arg;
      break;
    case 'n':
      verbose = no_output = 1;
      break;
    case 'o':
      outfile = arg;
      break;
    case 'P':
      period = arg;
      break;
    case 'p':
      project = arg;
      break;
    case 'q':
      qfile = arg;
      break;
    case 't':
      translation = arg;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'w':
      weboutput = 1;
      break;
    default:
      fprintf(stderr, "%s: unknown option -%c\n", invoke, opt);
      return 1;
    }
  return 0;
}

void help(void) { }
