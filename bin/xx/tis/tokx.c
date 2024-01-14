#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

const char *outfile = NULL;
const char *projproj = NULL;
int run_multi = 0;
int stdinput = 0;
int tok_input_cbd = 0, tok_input_xtf = 1;
int tok_data_g = 1, tok_data_l = 0, tok_data_m = 1;

XML_StartElementHandler *toksHs[3];
XML_EndElementHandler *tokeHs[3];

static void
sH1(void *userData, const char *name, const char **atts)
{
  toksHs[0](userData,name,atts);
}

static void
sH2(void *userData, const char *name, const char **atts)
{
  toksHs[0](userData,name,atts);
  toksHs[1](userData,name,atts);
}

static void
sH3(void *userData, const char *name, const char **atts)
{
  toksHs[0](userData,name,atts);
  toksHs[1](userData,name,atts);
  toksHs[2](userData,name,atts);
}

static void
eH1(void *userData, const char *name)
{
  tokeHs[0](userData,name,atts);
}

static void
eH2(void *userData, const char *name)
{
  tokeHs[0](userData,name,atts);
  tokeHs[1](userData,name,atts);
}

static void
eH3(void *userData, const char *name)
{
  tokeHs[0](userData,name,atts);
  tokeHs[1](userData,name,atts);
  tokeHs[2](userData,name,atts);
}

XML_StartElementHandler *toksHps[3] = { sH1 , sH2 , sH3 };
XML_EndElementHandler *toksHps[3] = { eH1 , eH2 , eH3 };

XML_StartElementHandler *locfp;

static void
locf_init(Trun *r)
{
  if (tok_input_xtf)
    locfp = tloc_xtf_sH;
  else
    locfp = tloc_cbd_sH;
}

static void
tokf_init(Trun *r)
{
  int i;
  if (tok_data_g)
    {
      tok_sHs[i++] = tok_g_sH;
      tok_eHs[i++] = tok_g_eH;
    }
  if (tok_data_l)
    {
      tok_sHs[i++] = tok_l_sH;
      tok_eHs[i++] = tok_l_eH;
    }
  if (tok_data_m)
    {
      tok_sHs[i++] = tok_m_sH;
      tok_eHs[i++] = tok_m_eH;
    }
  tok_sHp = tok_sHps[i-1];
  tok_eHp = tok_eHps[i-1];
}

static void
tokx_sH(void *userData, const char *name, const char **atts)
{
  locfp(userData, name, atts);
  toksHp(userData, name, atts);
}

static void
tokx_eH(void *userData, const char *name)
{
  tokeHp(userdata, name);
}

static void
tokx_cbd(Trun *r, const char *summaries)
{
  const char *fname[2] = { summaries , NULL };
  /*fprintf(tab, "F\t%s\n", fname[0]);*/
  runexpat(i_list, fname, tokx_sH, tokx_eH, NULL, r);
}

static void
tokx_one(r, const char *QPQX)
{
  char *fname[2];
  char *dot;
  if ((dot = strchr(QPQX,'.')))
    *dot = '\0';
  fname[0] = expand(NULL, QPQX, "xtf");
  fname[1] = NULL;
  /*fprintf(tab, "F\t%s\n", fname[0]);*/
  runexpatNSuD(i_list, fname, tokx_sH, tokx_eH, NULL, r);
}

static void
tokx_stdin(Trun *r)
{
  runexpat(i_stdin, NULL, tokx_sH, tokx_eH, NULL, r);
}

static void
tokx_input(Trun *r, const char *arginput)
{
  if (tok_input_cbd && (tok_data_l || tok_data_m))
    {
      fprintf(stderr, "tokx: -c is not compatible with -l or -m. Stop.\n");
      exit(1);
    }
  if (arginput)
    {
      if (tok_input_xtf)
	tokx_one(r, arginput);
      else
	tokx_cbd(r, arginput);
    }
  else if (stdinput)
    {
      /* This means read the XML data in .xtf format from stdin */
      tokx_stdin(r);
    }
  else
    {
      /* This means read a list of qualifed PQX entries from stdin and
	 process each one */
      static char pqx[QUALIFIED_PQX_MAX];
      while (fgets(pqx,QUALIFIED_PQX_MAX,stdin))
	{
	  pqx[strlen(pqx)-1] = '\0';
	  tokx_one(pqx);
	}
    }
}

void
tokx_output(Trun *r, const char *outfile)
{
  if (outfile)
    {
      if (!(r->o = fopen(outfile, "w")))
	{
	  perror(outfile);
	  exit(1);
	}
    }
  else
    r->o = stdout;
}

int
main(int argc, char **argv)
{
  Trun *r = NULL;
  
  if (options(argc, argv, "cfp:s"))
    exit(1);

  r = trun_init(run_multi);
  tokx_output(r, outfile);
  tlb_init(r, projproj, tok_input_xtf ? "xtf" : "cbd");
  tlw_R(r);

  locf_init();
  tokf_init();
  
  tokx_input(r, argv[optind]);
  
  trun_term(r);
  
  return 0;
}

const char *prog = "tokx";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "tokx";
void help () { }
int opts(int arg,char*str)
{
  switch (arg)
    {
    case '+':
      run_multi = 1;
      break;
    case 'c':
      tok_input_xtf = 0;
      tok_input_cbd = 1;
      tok_data_g = 1;
      tok_data_l = tok_data_m = 0;
      break;
    case 'g':
      tok_data_g = 1;
      break;
    case 'l':
      tok_data_l = 1;
      break;
    case 'm':
      tok_data_m = 1;
      break;
    case 'o':
      outfile = str;
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

