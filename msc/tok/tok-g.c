#include <unistd.h>
#include <string.h>
#include <oraccsys.h>
#include <runexpat.h>
#include <hash.h>
#include <pool.h>

/* Emulate ox -G output but do it from the XTF not from the ATF */

#undef strdup
extern char *strdup(const char *);
extern FILE *f_log;

static FILE *tab = NULL;

int printing = 0;
int xcl = 0;

static char pqx[128];
static char tlabel[128];
static char lid[128];
static char llabel[128];
static char project[1024];
static char last_oid[16];
static char last_spoid[16];
static char last_vs[16];

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
    }
  else if (!xcl && !strcmp(name, "l"))
    {
      strcpy(lid, get_xml_id(atts));
      strcpy(llabel, findAttr(atts, "label"));
      fprintf(tab,"L\t%d\t%s\t%s\n",pi_line,lid,llabel);
      printing = 1;
    }
  else if (!strcmp(name, "g:w"))
    fprintf(tab, "W\t%s\n", get_xml_id(atts));
  else if (printing && (!strcmp(name, "g:c") || !strcmp(name, "g:q")))
    {
#if 1
      strcpy(last_oid, findAttr(atts, "oid"));
      strcpy(last_spoid, findAttr(atts, "spoid"));
#else
      fprintf(tab, "g\t%s\t%s\t%s\t%s\t%s\t%s\n",
	      findAttr(atts, "oid"), "", "", "", name[2]=='q' ? "q" : "c", findAttr(atts, "form"));
#endif
      /* we don't process sub-sign/value of c/q */
      --printing;
    }
  else if (printing && (!strcmp(name, "g:v") || !strcmp(name, "g:s")))
    {
      strcpy(last_oid, findAttr(atts, "oid"));
      strcpy(last_spoid, findAttr(atts, "spoid"));
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
  if (printing && (!strcmp(name, "g:v") || !strcmp(name, "g:s")))
    {
#if 1
      char *g = (char*)charData_retrieve();
      const char *soid = (last_spoid ? last_spoid : last_oid);
      const char *foid = (last_spoid ? last_oid : "");
      if (name[2] == 'v')
	fprintf(tab, "%c.%s.%s:%s", name[2], soid, foid, g);
      else if (*foid)
	fprintf(tab, "%c.%s.%s", name[2], soid, foid);
      else
	fprintf(tab, "%c.%s", name[2], soid);
#else
      char *g = (char*)charData_retrieve();
      if (g[1] || 'x' != g[0])
	fprintf(tab, "g\t%s\t%s\t%s\t%s\t%s\n",
		last_oid, "", "", name[2]=='v' ? "v" : "s", g);
      *last_oid = '\0';
#endif
    }
  else if (!strcmp(name, "g:v"))
    strcpy(last_v, charData_retrieve());
  else if (!strcmp(name, "g:c") || !strcmp(name, "g:q"))
    {
      const char *soid = (last_spoid ? last_spoid : last_oid);
      const char *foid = (last_spoid ? last_oid : "");
      if (name[2] == 'q')
	{
	  if (*last_v)
	    fprintf(tab, "%c.%s.%s:%s", name[2], soid, foid, last_v);
	  else if (*foid)
	    fprintf(tab, "%c.%s.%s", name[2], soid, foid);
	  else
	    fprintf(tab, "%c.%s.%s", name[2], soid);
	}
      else
	fprintf(tab, "%c.%s.%s", name[2], soid, foid);
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
  if (argv[1])
    {
      strcpy(PQ, argv[1]);
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
int opts(int arg,char*str){ return 1; }
