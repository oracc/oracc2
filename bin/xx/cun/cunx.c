#include <oraccsys.h>
#include <xmlify.h>
#include <setjmp.h>
#include "runexpat.h"

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
const char *project;
const char *qfile;
const char *translation;

char *inpath, *outpath, *htmlpath;

FILE *outfp = NULL;

jmp_buf done;

extern int options(int, char*const*,const char*);
extern int optind;

int in_break = 0;
int in_l = 0;
int last_was_ellipsis = 0;
int word_count = 0;

typedef struct Cun_class
{
  const char *fnt;
  const char *mag;
  const char *scr;
} Cun_class;

Cun_class cun_defaults = { .fnt="noto" , .mag="100", .scr="middle" };
Cun_class *curr_cp = &cun_defaults;

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

const char *
cc_skip_prefix(const char *c)
{
  const char *ofs = "ofs-";
  if ('-' == c[0] && '-' == c[1])
    c += 2;
  if (!strncmp(c, ofs, strlen(ofs)))
    c += strlen(ofs);
  return c;
}

void
cun_head(FILE *fp, const char *n, Cun_class *cp)
{
  if (!weboutput) /* paradoxically, weboutput skips the html head/body because those are provided by P4 */
    {
      fprintf(fp,
	      "<html >"
	      "<head><meta charset=\"utf-8\"/>"
	      "<title>Cuneified %s</title>"
	      "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/fonts.css\"/>"
	      "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p4-cuneify.css\"/>"
	      "<script type=\"text/javascript\" src=\"/js/p4-cuneify.js\">&#160;</script>"
	      "</head><body onload=\"cuneify()\"", n);
    }
  fprintf(fp,
	  "<div id=\"p4Cuneify\" "
	  "data-cfy-fnt=\"%s\" data-cfy-mag=\"%s\" data-cfy-scr=\"%s\">",
	  cp->fnt, cp->mag, cp->scr);
  fprintf(outfp, "<h1 class=\"p3h2 border-top heading\">%s</h1><table class=\"cfy-table\">", n);
}

void
cun_foot(FILE *fp)
{
  fprintf(fp, "</table></div>");
  if (!weboutput)
    fprintf(fp, "</body></html>");
}

Cun_class *
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
ei_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "xcl"))
    longjmp(done, 0);

  if (!strcmp(name, "transliteration") || !strcmp(name, "composite"))
    {
      const char *xn = (ccp)xmlify((uccp)findAttr(atts, "n"));
      Cun_class *cp = cun_class(atts, curr_cp);
      *curr_cp = *cp;
      cun_head(outfp, xn, cp);
    }
  else
    {
      if (in_l)
	{
	  const char *utf8 = findAttr(atts, "g:utf8");
	  const char *gtype = findAttr(atts, "g:type");
	  const char *o = findAttr(atts, "g:o");
	  const char *c = findAttr(atts, "g:c");
	  const char *b = findAttr(atts, "g:break");

	  if (!in_break)
	    {
	      if (!strcmp(b, "missing") || 'x' == *utf8 || 'X' == *utf8)
		{
		  fprintf(outfp, "<span class=\"broken\">");
		  in_break = 1;
		}
	    }
	  else
	    {
	      if (strcmp(b, "missing") && *utf8 && 'x' != *utf8 && 'X' != *utf8)
		{
		  fprintf(outfp, "</span>");
		  in_break = 0;
		}
	    }
      
	  if (*utf8 || !strcmp(gtype, "ellipsis"))
	    {
	      if (strchr(o, '['))
		fprintf(outfp, "<span class=\"roman\">[</span>");
      
	      if ('x' == *utf8 || 'X' == *utf8)
		{
		  fprintf(outfp, "<span class=\"roman gray\">×</span>");
		  last_was_ellipsis = 0;
		}
	      else if (!*utf8)
		{
		  fprintf(outfp, "<span class=\"roman\">%s. . .</span>", last_was_ellipsis ? " " : "");
		  last_was_ellipsis = 1;
		}
	      else
		{
		  fprintf(outfp, "<span class=\"cfy-cun\">%s</span>", utf8);
		  last_was_ellipsis = 0;
		}
	  
	      if (strchr(c, ']'))
		fprintf(outfp, "<span class=\"roman\">]</span>");
	    }
	  else if (!strcmp(name, "g:w"))
	    {
	      if (++word_count && !last_was_ellipsis)
		fprintf(outfp, "<span class=\"cfy-ws\"> </span>");
	    }
	}
      else
	{
	  if (!strcmp(name, "l"))
	    {
	      fprintf(outfp, "<tr><td class=\"cuneify-label\">%s</td><td>", findAttr(atts, "label"));
	      last_was_ellipsis = word_count = 0;
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
      if (in_break)
	{
	  fprintf(outfp, "</span>");
	  in_break = 0;
	}
      fprintf(outfp, "</td></tr>");
      in_l = last_was_ellipsis = 0;
    }
#if 0
  else if (!strcmp(name, "g:w"))
    {
      ;
    }
#endif
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
  options(argc, argv, "bfi:l:no:p:q:st:vw");

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
	  if (!setjmp(done))
	    runexpat(i_list, f, ei_sH, ei_eH);
	  cun_foot(outfp);
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
