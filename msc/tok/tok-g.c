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
int tok_cbd = 0;
int tok_xtf = 1;
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

List *wg = NULL;
int wg_index = 0;

static char role;
static char roletext[32];

struct wg
{
  char type;
  char role;
  char text[1024];
  char oid[16];
  char sign[1024];
  char spoid[16];
  char spsign[1024];
  char roletext[32];
  int index;
  int last;
};

struct wg *
wgp_get(int wgx)
{
  static struct wg *wgmem = NULL;
  static int wg_alloced = 0;
  if (wgx == -1 && wgmem)
    {
      memset(wgmem, '\0', wg_alloced * sizeof(struct wg));
      return NULL;
    }
  else
    {
      if (wgx == wg_alloced)
	{
	  wg_alloced += 16;
	  wgmem = realloc(wgmem, wg_alloced * sizeof(struct wg));
	}
      return &wgmem[wgx];
    }
}

void
wg_add(char type, const char *oid, const char *sign, const char *spoid, const char *spsign)
{
  struct wg *wgp = wgp_get(++wg_index);
  if (oid)
    strcpy(wgp->oid, oid);
  if (sign)
    strcpy(wgp->sign, sign);
  if (spoid)
    strcpy(wgp->spoid, spoid);
  if (spsign)
    strcpy(wgp->spsign, spsign);
  if (role)
    {
      wgp->role = role;
      if (*roletext)
	strcpy(wgp->roletext, roletext);
    }
  wgp->index = wg_index;
  list_add(wg, (void*)(uintptr_t)wg_index);
}

void
wgp_last(void)
{
  if (wg_index >= 0)
    {
      struct wg *wgp = wgp_get(wg_index);
      wgp->last = 1;
    }
}

void
wgp_text(const char *t)
{
  if (wg_index >= 0)
    {
      struct wg *wgp = wgp_get(wg_index);
      strcpy(wgp->text, t);
    }
}

void
wgp_print(void)
{
  int i;
  for (i = 0; i <= wg_index; ++i)
    {
      struct wg *wgp = wgp_get(i);
      fprintf(tab, "%s\t%s\n", wgp->oid, wgp->text);
    }
}

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
  else if (!strcmp(name, "xcl"))
    xcl = 1;
}
	    
static void
sH(void *userData, const char *name, const char **atts)
{
  loc(userData, name, atts);
  if ('g' == name[0] && ':' == name[1])
    {
      switch (name[2])
	{
	case 'w':
	  wg = list_create(LIST_SINGLE);
	  wg_index = -1;
	  wgp_get(-1);
	  break;
	case 'd':
	  role = 'd';
	  strcpy(roletext, findAttr(atts, "g:role"));
	  break;
	case 'v':
	  wg_add('v',
		 findAttr(atts, "oid"),
		 findAttr(atts, "g:sign"),
		 NULL, NULL);
	  break;
	default:
	  break;
	}
    }
}

static void
eH(void *userData, const char *name)
{
  if ('g' == name[0] && ':' == name[1])
    {
      switch (name[2])
	{
	case 'w':
	  wgp_last();
	  wgp_print();
	  break;
	case 'd':
	  role = '\0';
	  break;
	case 'v':
	  wgp_text(charData_retrieve());
	  break;
	default:
	  (void)charData_retrieve();
	  break;
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

  options(argc, argv, "cp:");

  if (projproj)
    fprintf(tab, "P\t%s\n", projproj);

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
    case 'c':
      tok_xtf = 0;
      tok_cbd = 1;
      break;
    case 'p':
      projproj = str;
      break;
    default:
      return 1;
    }
  return 0;
}

