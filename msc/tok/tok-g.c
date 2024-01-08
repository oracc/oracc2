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
int stdinput = 0;
int tok_cbd = 0;
int tok_xtf = 1;
int xcl = 0;

static const char *projproj = NULL;

static char pqx[128];
static char tlabel[128];
static char lid[128];
static char llabel[128];
static char project[1024];
static char word_lang[16];

List *wg = NULL;
int wg_index = 0;
int no_d_index = 0;

static char role;
static char roletext[32];

struct wg
{
  char gdltype;
  char role; 		/* d(eterminative) g(loss) p(unctuation) w(ord-constituent) u(ndetermined) */
  char roletype;	/* role=d: s(emantic) p(honetic) v(ariant) u(ndetermined)
			   role=g: t(ranslation) v(ariant)
			   role=p: b(ullet) d(ivider) s(urrogate) u(ndetermined)
			   role=w|u: i(gnored)
			 */
  char type;		/* i(deogram)
			   m(orpheme) M(ixed-morpho-ideo/syll)
			   l(ogogram[secondary-language])
			   s(yllable)
			   u(ndetermined) */
  char position;	/* i(ndependent) b(eginning) m(iddle) e(nd) u(ndetermined) */
  char no_d_position;   /* i(ndependent) b(eginning) m(iddle) e(nd) u(ndetermined) */
  char asltype[3]; 	/* pc pe sl */
  char lang[16]; 	/* language for word */
  char logolang[16];	/* secondary language for logogram */
  char soid[16];	/* sign-oid */
  char sname[1024];	/* sign-name */
  char foid[16];	/* form-oid */
  char fname[1024];	/* form-name */
  char value[1024];	/* value when gdltype == 'v' */
  int index;		/* Index of grapheme in word, counting from 1; 0 = no g:w parent */
  int last;		/* 1 = final grapheme in word */
  int no_d_index;	/* Index ignoring initial determinatives */
  int no_d_last;	/* 1 = final grapheme in word ignoring final determinatives */
};

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
g_signature(struct wg *wgp,const char *id_sig_sep)
{
  fprintf(tab, "%s.%s.%s%s@%s%%%s:%c/%s-%s-%s#%c%c%%%s:#%c%%%s:#%c%d#%c%d",
	  wgp->soid, wgp->foid, wgp->value,
	  id_sig_sep,
	  project, wgp->asltype, wgp->gdltype ? wgp->gdltype : 'u',
	  wgp->sname, wgp->fname, wgp->value,
	  wgp->role ? wgp->role : 'u', wgp->roletype ? wgp->roletype : 'u', wgp->lang,
	  wgp->type, wgp->logolang,
	  wgp->position ? wgp->position : 'u', wgp->index,
	  wgp->no_d_position ? wgp->no_d_position : 'u', wgp->no_d_index
	  );
}

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
	  memset((void*)(wgmem + (wg_alloced-16)), '\0', 16 * sizeof(struct wg));
	}
      return &wgmem[wgx];
    }
}

void
wg_add(char type, const char *oid, const char *sign, const char *spoid, const char *spsign, const char *logolang)
{
  struct wg *wgp = wgp_get(++wg_index);
  wgp->gdltype = type;
  wgp->role = 'w'; 	/* may be reset later */
  wgp->roletype = 'i'; 	/* ditto */
  wgp->type = 'u'; 	/* ditto */
  if (spoid)
    {
      strcpy(wgp->soid, spoid);
      strcpy(wgp->sname, spsign);
      strcpy(wgp->foid, oid);
      strcpy(wgp->fname, sign);
    }
  else
    {
      strcpy(wgp->soid, oid);
      strcpy(wgp->sname, sign);
    }
  if (*word_lang)
    {
      strcpy(wgp->lang, word_lang);
      strcpy(wgp->asltype, sl_of(word_lang));
    }
  if (role)
    {
      wgp->role = role;
      if (*roletext)
	wgp->roletype = *roletext;
    }
  if (*logolang)
    {
      wgp->type = 'l';
      strcpy(wgp->logolang, logolang);
    }
  else
    wgp->type = 'u';
  wgp->index = 1 + wg_index;
  if ('d' != role)
    wgp->no_d_index = 1 + ++no_d_index;
  list_add(wg, (void*)(uintptr_t)wg_index);
}

void
wgp_set_positions(void)
{
  int i;
  for (i = 0; i <= wg_index; ++i)
    {
      struct wg *wgp = wgp_get(i);
      if (wgp->index > 1)
	{
	  if (wgp->last)
	    {
	      if (wgp->index == 1)
		wgp->position = 'i';
	      else
		wgp->position = 'e';
	    }
	  else
	    wgp->position = 'm';
	}
      else
	{
	  if (wgp->last)
	    wgp->position = 'i';
	  else
	    wgp->position = 'b';
	}	  
      if (wgp->no_d_index > 1)
	{
	  if (wgp->no_d_last)
	    {
	      if (wgp->no_d_index == 1)
		wgp->no_d_position = 'i';
	      else
		wgp->no_d_position = 'e';
	    }
	  else
	    wgp->no_d_position = 'm';
	}
      else
	{
	  if (wgp->no_d_last)
	    wgp->no_d_position = 'i';
	  else
	    wgp->no_d_position = 'b';
	}
    }
}

void
wgp_last(void)
{
  if (wg_index >= 0)
    {
      struct wg *wgp = wgp_get(wg_index);
      if (wgp)
	{
	  wgp->last = 1;
	  if (wg_index != no_d_index && no_d_index >= 0)
	    {
	      wgp = wgp_get(no_d_index);
	      wgp->no_d_last = 1;
	    }
	  else
	    wgp->no_d_last = 1;
	}
      wgp_set_positions();
    }
}

void
wgp_value(const char *t)
{
  if (wg_index >= 0)
    {
      struct wg *wgp = wgp_get(wg_index);
      strcpy(wgp->value, t);
    }
}

void
wgp_print(void)
{
  int i;
  for (i = 0; i <= wg_index; ++i)
    {
      struct wg *wgp = wgp_get(i);
      g_signature(wgp, "\t");
      fputc('\n', tab);
    }
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
	  wg_index = no_d_index = -1;
	  break;
	case 'd':
	  role = 'd';
	  strcpy(roletext, findAttr(atts, "g:role"));
	  break;
	case 's':
	case 'v':
	  wg_add(name[2],
		 findAttr(atts, "oid"),
		 findAttr(atts, "g:sign"),
		 NULL, NULL,
		 findAttr(atts, "g:logolang"));
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
	  wgp_get(-1);
	  *word_lang = '\0';
	  break;
	case 'd':
	  role = '\0';
	  break;
	case 'v':
	  wgp_value(charData_retrieve());
	  break;
	case 's':
	  /* this is done in the start tag */
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

  options(argc, argv, "cp:s");

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
    case 's':
      stdinput = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

