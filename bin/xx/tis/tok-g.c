#include <unistd.h>
#include <string.h>
#include <oraccsys.h>
#include <runexpat.h>
#include <xmd.h>
#include <gsig.h>

/* Emulate ox -G output but do it from the XTF not from the ATF */

extern FILE *f_log;

static FILE *tab = NULL;

static struct gsb_input *gi = NULL;
static struct gsb_word *gswp = NULL;
static const char *projproj;

int form_with_w = 0;
int printing = 0;
int stdinput = 0;
int tok_cbd = 0;
int tok_xtf = 1;
int xcl = 0;

static void
tok_mds(const char *project, const char *pqx)
{
  xmd_init();
  Hash *m = xmd_load(project, pqx);
  if (m)
    {
      ccp period = hash_find(m, (ucp)"period");
      ccp periodc = hash_find(m, (ucp)xmd_name_c("period"));
      ccp sphase = hash_find(m, (ucp)"sphase");
      ccp sphasec = NULL;
      if (!period || !*period)
	{
	  period = "none";
	  periodc = "999999";
	}
      if (!sphase || !*sphase)
	{
	  sphase = period;
	  sphasec = periodc;
	}
      else
	sphasec = hash_find(m, (ucp)xmd_name_c("sphase"));
      fprintf(tab, "K\tperiod\t%06d/%s\n", atoi(periodc), period);
      fprintf(tab, "K\tsphase\t%06d/%s\n", atoi(sphasec), sphase);
    }
  xmd_term();
}

typedef void (*loc_func)(void *userData, const char *name, const char **atts);
loc_func loc;

static void
loc_cbd(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "articles"))
    fprintf(tab, "G\t%s\n", get_xml_lang(atts));
  else if (!strcmp(name, "summary"))
    fprintf(tab, "W\t%s\t%s\n", findAttr(atts, "oid"), findAttr(atts, "n"));
}

static void
loc_xtf(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "transliteration")
      || !strcmp(name, "composite")
      || !strcmp(name, "score"))
    {
      const char *pqx = get_xml_id(atts);
      const char *tlabel = findAttr(atts, "n");
      const char *project = findAttr(atts, "project");
      if (!gi->textproj || strcmp(gi->textproj, project))
	gi->textproj = (ccp)hpool_copy((ucp)project, gi->p);
      printing = 1;
      xcl = 0;
      fprintf(tab,"T\t%s\t%s\t%s\n",project,pqx,tlabel);
      tok_mds(project, pqx);
    }
  else if (!xcl && !strcmp(name, "l"))
    {
      const char *lid = get_xml_id(atts);
      const char *llabel = findAttr(atts, "label");
      fprintf(tab,"L\t%d\t%s\t%s\n",pi_line,lid,llabel);
      printing = 1;
    }
  else if (!strcmp(name, "g:w") || !strcmp(name, "g:nonw"))
    {
      fprintf(tab, "W\t%s\n", get_xml_id(atts));
    }
  else if (!strcmp(name, "xcl"))
    xcl = 1;
}
	    
static void
sH(void *userData, const char *name, const char **atts)
{
  char form[1024];
  *form = '\0';
  (void)charData_retrieve();
  loc(userData, name, atts);
  if ('g' == name[0] && ':' == name[1])
    {
      if (!name[3])
	{
	  switch (name[2])
	    {
	    case 'w':
	      gswp = gsb_word_init(gi);
	      gswp->lang = (ccp)hpool_copy((uccp)get_xml_lang(atts), gswp->in->p);
	      gswp->form = (ccp)hpool_copy((uccp)findAttr(atts, "form"), gswp->in->p);
	      break;
	    case 'd':
	      gswp->in->role = 'd';
	      gswp->in->roletext = (ccp)hpool_copy((uccp)findAttr(atts, "g:role"), gswp->in->p);
	      break;
	    case 'n':
	      gswp->in->in_n = 1;
	      gsb_add(gswp,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      gswp->lang,
		      findAttr(atts, "g:logolang"));
	      gsb_value(gswp, findAttr(atts, "form"));
	      break;
	    case 'p':
	      gsb_add(gswp,
		      name[2],
		      findAttr(atts, "g:type"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      gswp->lang,
		      findAttr(atts, "g:logolang"));
	      gsb_punct(gswp, findAttr(atts, "g:type"));
	      gswp->in->in_p = 1;
	      break;
	    case 'q':
	      gsb_add(gswp,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      findAttr(atts, "spoid"),
		      findAttr(atts, "spform"),
		      gswp->lang,
		      findAttr(atts, "g:logolang"));
	      gswp->in->in_q = 1;
	      break;
	    case 'c':
	      gswp->in->in_c = 1;
	      gsb_add(gswp,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      gswp->lang,
		      findAttr(atts, "g:logolang"));
	      break;
	    case 's':
	    case 'v':
	      if (!gswp->in->in_n && !gswp->in->in_p && !gswp->in->in_q)
		gsb_add(gswp,
			name[2],
			NULL,
			findAttr(atts, "oid"),
			findAttr(atts, "g:sign"),
			NULL, NULL,
			gswp->lang,
			findAttr(atts, "g:logolang"));
	      break;
	    case 'b':
	    case 'r':
	      break;
	    default:
	      break;
	    }
	}
      else
	{
	  if (!strcmp(name, "g:nonw"))
	    {
	      gswp = gsb_word_init(gi);
	    }
	}
    }
}

static void
eH(void *userData, const char *name)
{
  if ('g' == name[0] && ':' == name[1])
    {
      if (!name[3])
	{
	  switch (name[2])
	    {
	    case 'w':
	      gsb_last(gswp);
	      gsb_show(stdout, gswp, form_with_w);
	      gsb_word_reset(gswp);
	      break;
	    case 'c':
	      gswp->in->in_c = 0;
	      gsb_c_last(gswp);
	      gswp->curr_c_wgp = NULL;
	      break;
	    case 'd':
	      gswp->in->role = '\0';
	      gswp->in->roletext = "";
	      break;
	    case 'n':
	      gswp->in->in_n = 0;
	      break;
	    case 'p':
	      gswp->in->in_p = 0;
	      break;
	    case 'q':
	      gswp->in->in_q = 0;
	      break;
	    case 's':
	      if (!gswp->in->in_n && !gswp->in->in_q)
		gsb_sign(gswp, charData_retrieve());
	      break;
	    case 'v':
	      if (!gswp->in->in_n && !gswp->in->in_q)
		gsb_value(gswp, charData_retrieve());
	      break;
	    case 'b':
	    case 'r':
	      break;
	    default:
	      (void)charData_retrieve();
	      break;
	    }
	}
      else
	{
	  if (!strcmp(name, "g:nonw"))
	    {
	      gsb_last(gswp);
	      gsb_show(stdout, gswp, form_with_w);
	      gsb_word_reset(gswp);
	      gswp->lang = "";
	    }
	  else
	    (void)charData_retrieve();
	}
    }
  else
    (void)charData_retrieve();
}

static void
tok_g_one(char *PQ)
{
  char *fname[2];
  char *dot;
  if ((dot = strchr(PQ,'.')))
    *dot = '\0';
  fname[0] = expand(NULL, PQ, "xtf");
  fname[1] = NULL;
  fprintf(tab, "F\t%s\n", fname[0]);
  runexpat(i_list, fname, sH, eH);
}

static void
tok_g_stdin(void)
{
  runexpat(i_stdin, NULL, sH, eH);
}

static void
tok_g_cbd(char *summary)
{
  char *fname[2];
  fname[0] = summary;
  fname[1] = NULL;
  fprintf(tab, "F\t%s\n", fname[0]);
  runexpat(i_list, fname, sH, eH);
}

int
main(int argc, char **argv)
{
  char PQ[512];

  tab = stdout;

  if (options(argc, argv, "cfp:s"))
    exit(1);

  gi = gsb_input_init();
  
  if (projproj)
    {
      gi->projproj = (ccp)hpool_copy((uccp)projproj, gi->p);
      fprintf(tab, "P\t%s\n", gi->projproj);
    }

  if (tok_xtf)
    {
      fputs("Y\txtf\n", tab);      
      loc = loc_xtf;
    }
  else
    {
      fputs("Y\tcbd\n", tab);
      loc = loc_cbd;
    }

  if (argv[optind])
    {
      if (tok_xtf)
	{
	  strcpy(PQ, argv[optind]);
	  tok_g_one(PQ);
	}
      else
	tok_g_cbd(argv[optind]);
    }
  else if (stdinput)
    {
      tok_g_stdin();
    }
  else
    {
      while (fgets(PQ,512,stdin))
	{
	  PQ[strlen(PQ)-1] = '\0';
	  tok_g_one(PQ);
	}
    }

  gsb_input_term(gi);
  
  return 0;
}

const char *prog = "tok-g";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "labeltable <XTF >TAB";
void help () { }
int opts(int arg,char*str)
{
  switch (arg)
    {
    case 'c':
      tok_xtf = 0;
      tok_cbd = 1;
      break;
    case 'f':
      form_with_w = 1;
      break;
    case 'p':
      projproj = str;
      break;
    case 's':
      stdinput = 1;
      break;
    default:
      return 1;
    }
  return 0;
}
