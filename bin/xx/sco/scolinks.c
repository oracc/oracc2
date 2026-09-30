#include <oraccsys.h>
#include <roco.h>
#include <runexpat.h>

const char *current_proj = NULL;
const char *current_pqid = NULL;
const char *output_fn = NULL;
FILE *outfp = NULL;
int stdin_input = 0;

/* load the .tlx file from PROJECT, PQX args into the HASH arg */
void
index_text(const char *project, const char *pqx, Hash *links, Pool *p)
{
  const char *tlxfile = expand(project, pqx, ".tlx");
  Roco *r = roco_load1(tlxfile);
  int i;
  for (i = 0; i < r->nlines; ++i)
    {
      char buf[strlen(project)+strlen(pqx)+strlen((ccp)r->rows[i][0])+3];
      sprintf(buf, "%s:%s~%s", project, pqx, r->rows[i][0]);
      hash_add(links, pool_copy((uccp)buf, p), r->rows[i][1]);
    }
}

void
pp_def(const char **atts)
{
}

void
pp_pll(const char **atts)
{
}

void
pp_ref(char type, const char **atts)
{
}

void
pp_src(const char **atts)
{
}

void
pp_xml_begin(void)
{
  fputs("<linkbase>", outfp);
}
void
pp_xml_end(void)
{
  fputs("</linkbase>", outfp);
}

void
process_protocol(const char **atts)
{
  const char *subt = findAttr(atts, "subt");
  switch (*subt)
    {
    case 'd':
      pp_def(atts);
      break;
    case 'p':
      pp_pll(atts);
      break;
    case 's':
      pp_src(atts);
      break;
    case '<':
    case '>':
    case '|':
    case '+':
      pp_ref(*subt, atts);
      break;
    default:
      break;
    }
}

void
sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "protocol"))
    {
      const char *type = findAttr(atts, "type");
      if (type && !strcmp(type, "link"))
	process_protocol(atts);
    }
}

void
eH(void *userData, const char *name)
{
}

void
proj_pqid(const char *str)
{
  char *fn = strdup(str), *colon;
  colon = strchr(fn, ':');
  if (colon)
    {
      current_proj = fn;
      *colon++ = '\0';
      current_pqid = colon;
    }
  else
    {
      fprintf(stderr, "scolinks: input files must be specified as PROJECT:PQID. Stop.\n");
      exit(1);
    }
}

int
main(int argc, char **argv)
{
  char PQ[512];
  const char *fname[2] = { NULL, NULL };
  options(argc, argv, "i:o:p:s");
  if (output_fn)
    outfp = freopen(output_fn, "w", stdout);
  else
    outfp = stdout;
  if (!outfp)
    {
      fprintf(stderr, "scolinks: unable to write to %s. Stop.\n", output_fn);
      exit(1);
    }

  if (stdin_input)
    {
      if (!current_pqid || !current_proj)
	{
	  fprintf(stderr, "scolinks: must give -p [PROJECT] -i [PQID] with -s option. Stop.\n");
	  exit(1);
	}
      pp_xml_begin();
      runexpat(i_stdin, NULL, sH, eH);
      pp_xml_end();
    }
  else if (argv[optind])
    {
      proj_pqid(argv[optind]);
      fname[0] = expand(NULL, argv[optind], "xtf");
      pp_xml_begin();
      runexpat(i_list, fname, sH, eH);
      pp_xml_end();
    }
  else
    {
      pp_xml_begin();
      while (fgets(PQ,512,stdin))
	{
	  proj_pqid(PQ);
	  fname[0] = expand(NULL, PQ, "xtf");
	  runexpat(i_list, fname, sH, eH);
	}
      pp_xml_end();
    }
  return 0;
}

const char *prog = "scolinks";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "scolinks [-p PROJ -i PQID -s ] | QID | [QIDS-FROM-STDIN]";
void help (void) { }
int
opts(int arg,const char*str)
{
  switch (arg)
    {
    case 'i': current_pqid = str; break;
    case 'o': output_fn = str; break;
    case 'p': current_proj = str; break;
    case 's': stdin_input = 1; break;
    default: return 1; break;
    }
  return 0;
}
