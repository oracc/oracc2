#include <unistd.h>
#include <string.h>
#include <oraccsys.h>
#include <runexpat.h>
#include <xmd.h>

/* Emulate ox -G output but do it from the XTF not from the ATF */

#undef strdup
extern char *strdup(const char *);
extern FILE *f_log;

static FILE *tab = NULL;

int printing = 0;
int xcl = 0;

static const char *projproj = NULL;

static char pqx[128];
static char tlabel[128];
static char lid[128];
static char llabel[128];
static char project[1024];
static char last_oid[16];
static char last_spoid[16];
static char last_spform[1024];
static char last_form[1024];
static char last_v[16];
static char last_s[1024];
static char word_lang[16];

static char *
sl_of(const char *lang)
{
  if (lang)
    {
      if ('q' == lang[0] && 'p' == lang[1])
	return 'c' == lang[2] ? "pc" : "pe";
      else
	return "sl";
    }
  else
    return "sl";
}

static void
sign_signature(void)
{
  const char *s = (*last_spform ? last_spform : (*last_form ? last_form : last_s));
  const char *f = (*last_spform ? (*last_form ? last_form : last_s) : "");
  fprintf(tab, "\t@%s%%%s:%s-%s-%s\n", project, sl_of(word_lang),s,f,last_v);
  *last_form = *last_spform = '\0';
}

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

static void
sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "transliteration") || !strcmp(name, "composite"))
    {
      strcpy(pqx, get_xml_id(atts));
      strcpy(tlabel, findAttr(atts, "n"));
      strcpy(project, findAttr(atts, "project"));
      printing = 1;
      xcl = 0;
      fprintf(tab,"T\t%s\t%s\t%s\n",project,pqx,tlabel);
      tok_mds(project, pqx);
    }
  else if (!xcl && !strcmp(name, "l"))
    {
      strcpy(lid, get_xml_id(atts));
      strcpy(llabel, findAttr(atts, "label"));
      fprintf(tab,"L\t%d\t%s\t%s\n",pi_line,lid,llabel);
      printing = 1;
    }
  else if (!strcmp(name, "g:w"))
    {
      strcpy(word_lang, get_xml_lang(atts));
      fprintf(tab, "W\t%s\n", get_xml_id(atts));
    }
  else if (printing && (!strcmp(name, "g:c") || !strcmp(name, "g:q") || !strcmp(name, "g:n")))
    {
      strcpy(last_oid, findAttr(atts, "oid"));
      strcpy(last_spoid, findAttr(atts, "spoid"));
      strcpy(last_spform, findAttr(atts, "spform"));
      if ('c' == name[2])
	strcpy(last_form, findAttr(atts, "form"));
      else
	strcpy(last_form, findAttr(atts, "g:sign"));
      *last_s = *last_v = '\0';
      /* we don't process sub-sign/value of c */
      if ('c' == name[2])
	printing = 0;
    }
  else if (printing && (!strcmp(name, "g:v") || !strcmp(name, "g:s")))
    {
      strcpy(last_oid, findAttr(atts, "oid"));
      strcpy(last_s, findAttr(atts, "g:sign"));
#if 0
      if (!*last_form) /* don't overwrite g:q form */
	strcpy(last_form, findAttr(atts, "g:sign"));
#endif
    }
  else if (!strcmp(name, "xcl"))
    {
      printing = 0;
      xcl = 1;
    }  
}

static void
eH(void *userData, const char *name)
{
  if (!strcmp(name, "g:v"))
    strcpy(last_v, charData_retrieve());
  else if (!strcmp(name, "g:s"))
    strcpy(last_s, charData_retrieve());
  if (printing && (!strcmp(name, "g:v") || !strcmp(name, "g:s")))
    {
      if (*last_oid)
	{
	  if (strcmp(last_v, "x"))
	    {
	      const char *soid = (*last_spoid ? last_spoid : last_oid);
	      const char *foid = (*last_spoid ? last_oid : "");
	      if (name[2] == 'v')
		{
		  if (*foid)
		    fprintf(tab, "%s.%s:%s", soid, foid, last_v);
		  else
		    fprintf(tab, "%s:%s", soid, last_v);
		}
	      else if (*foid)
		fprintf(tab, "%s.%s", soid, foid);
	      else
		fprintf(tab, "%s", soid);
	      sign_signature();
	    }
	  *last_v = *last_s = '\0';
	}
    }
  else if (!strcmp(name, "g:c") || !strcmp(name, "g:q") || !strcmp(name, "g:n"))
    {
      if (*last_oid && (*last_v || *last_s))
	{
	  const char *soid = (*last_spoid ? last_spoid : last_oid);
	  const char *foid = (*last_spoid ? last_oid : "");
	  if (name[2] == 'q')
	    {
	      if (*last_v)
		fprintf(tab, "%s.%s:%s", soid, foid, last_v);
	      else if (*foid)
		fprintf(tab, "%s.%s", soid, foid);
	      else
		fprintf(tab, "%s", soid);
	    }
	  else if (*foid)
	    fprintf(tab, "%s.%s", soid, foid);
	  else
	    fprintf(tab, "%s", soid);
	  sign_signature();
	}
      *last_v = *last_s = '\0';
      ++printing;
    }
  else
    charData_discard();
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

int
main(int argc, char **argv)
{
  char PQ[512];
  tab = stdout;

  options(argc, argv, "p:");

  if (projproj)
    fprintf(tab, "P\t%s\n", projproj);
    
  if (argv[optind])
    {
      strcpy(PQ, argv[optind]);
      tok_g_one(PQ);
    }
  else
    {
      while (fgets(PQ,512,stdin))
	{
	  PQ[strlen(PQ)-1] = '\0';
	  tok_g_one(PQ);
	}
    }
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
    case 'p':
      projproj = str;
      break;
    default:
      return 1;
    }
  return 0;
}

