#include <oraccsys.h>
#include <roco.h>
#include <runexpat.h>

const char *current_proj = NULL;
const char *current_pqid = NULL;
char *last_lid = NULL;
const char *output_fn = NULL;
FILE *outfp = NULL;
int stdin_input = 0;

Hash *badatf = NULL;
Hash *defs = NULL;
Hash *indexed = NULL;
Hash *linkindex = NULL;
Hash *prefs = NULL;
Hash *parallels_ok = NULL;
Hash *seen = NULL;
Hash *sources_ok = NULL;
Hash *srefs = NULL;
Hash *symdefs = NULL;
Pool *p = NULL;

void
qid_prj_pqx(const char *qid, const char **prj, const char **pqx)
{
  char *fn = strdup(qid), *colon;
  colon = strchr(fn, ':');
  if (colon)
    {
      *prj = fn;
      *colon++ = '\0';
      *pqx = colon;
    }
  else
    {
      fprintf(stderr, "scolinks: QID must be specified as PROJECT:PQX. Stop.\n");
      exit(1);
    }
}

/* load the .tlx file text arg into the HASH arg */
void
index_text(const char *text, Hash *links, Pool *p, const char **prj, const char **pqx)
{
  const char *tlxfile = expand(NULL, text, ".tlx");
  if (!access(tlxfile, R_OK))
    {
      qid_prj_pqx(text, prj, pqx);
      Roco *r = roco_load1(tlxfile);
      int i;
      for (i = 0; i < r->nlines; ++i)
	{
	  char buf[strlen(*prj)+strlen(*pqx)+strlen((ccp)r->rows[i][0])+3];
	  sprintf(buf, "%s:%s~%s", *prj, *pqx, r->rows[i][0]);
	  hash_add(links, pool_copy((uccp)buf, p), r->rows[i][1]);
	}
      hash_add(indexed, (uccp)text, "");
    }
  else
    {
      hash_add(badatf, (uccp)text, "");
    }
}

void
pp_def(const char **atts)
{
  /* variable names match harvest-links.plx process_protocol / def */
  const char *sym = (ccp)pool_copy((uccp)findAttr(atts, "sig"), p);
  const char *text = (ccp)pool_copy((uccp)findAttr(atts, "qid"), p);
  if (text && *text)
    {
      const char *prj = NULL;
      const char *pqx = NULL;
      index_text(text, linkindex, p, &prj, &pqx);
      hash_add(defs, (uccp)sym, (void*)text);
      char symkey[strlen(current_proj)+strlen(current_pqid)+strlen(pqx)+3];
      sprintf(symkey, "%s:%s:%s", current_proj, current_pqid, pqx);
      hash_add(symdefs, pool_copy((uccp)symkey, p), (void*)sym);
      /* TODO: harvest-links.plx checked if PQID matched current text,
	 i.e., if "$prj:pqx" == "current_proj:current_pqid */
    }
}

void
pp_pll(const char **atts)
{
  const char *text = (ccp)pool_copy((uccp)findAttr(atts, "qid"), p);
  if (text && *text)
    {
      const char *prj = NULL;
      const char *pqx = NULL;
      qid_prj_pqx(text, &prj, &pqx);

      char pending[strlen(prj)+strlen(pqx)+strlen(text)+3];
      sprintf(pending, "%s:%s=%s", prj, pqx, text);
      hash_add(seen, pool_copy((uccp)pending, p), "");
      sprintf(pending, "%s=%s:%s", text, prj, pqx);
      hash_add(seen, pool_copy((uccp)pending, p), "");

      char pref[strlen(prj)+strlen(pqx)+2];
      sprintf(pref, "%s:%s", prj, pqx);
      const char *p_pref = (ccp)pool_copy((uccp)pref, p);
      hash_add(seen, (uccp)p_pref, (void*)text);
      hash_add(seen, (uccp)text, (void*)p_pref);

      char pok[strlen(text)+strlen(pref)+3];
      sprintf(pok, "%s->%s", text, pref);
      hash_add(parallels_ok, pool_copy((uccp)pok, p), "");
    }
}

void
pp_ref(char type, const char **atts)
{
  const char *qid = findAttr(atts, "qid");
  const char *sig = findAttr(atts, "sig");
  const char *label = (ccp)pool_copy((uccp)findAttr(atts, "label"), p);
  const char *symdef = NULL;
  const char *symprj = NULL, *sympqx = NULL;
  if (qid && *qid)
    {
      char *colon = strchr(qid, ':');
      if (colon)
	{
	  qid_prj_pqx(qid, &symprj, &sympqx);
	}
      else
	{
	  symprj = "cdli";
	  sympqx = qid;
	}
    }
  else if (sig && *sig && (symdef = hash_find(defs, (uccp)sig)))
    {
      qid_prj_pqx(symdef, &symprj, &sympqx);
    }
  if (symprj)
    {
      char xlink[strlen(symprj)+strlen(sympqx)+strlen(label)+3];
      sprintf(xlink, "%s:%s~%s", symprj, sympqx, label);

      char fref_buf[strlen(current_proj)+strlen(current_pqid)+2], *fref;
      sprintf(fref_buf, "%s:%s", current_proj, current_pqid);
      fref = (char*)pool_copy((uccp)fref_buf, p);

      char tref_buf[strlen(symprj)+strlen(sympqx)+2], *tref;
      sprintf(tref_buf, "%s:%s", symprj, sympqx);
      tref = (char*)pool_copy((uccp)tref_buf, p);

      const char *xlx = NULL;
      
      if ((xlx = hash_find(linkindex, (uccp)xlink)))
	{
	  
	}
      else
	{
	  const char *pending = NULL;
	  const char *symdef = hash_find(defs, (uccp)sig);
	  switch (type)
	    {
	    case '>':
	      {
		/* SREFS */
		char sok[strlen(fref)+strlen(tref)+3];
		sprintf(sok, "%s->%s", fref, tref);
		hash_add(sources_ok, pool_copy((uccp)sok, p), "");
	      }
	      break;
	    case '|':
	      {
		/* PREFS */
		char pok[strlen(fref)+strlen(tref)+3];
		sprintf(pok, "%s->%s", fref, tref);
		hash_add(parallels_ok, pool_copy((uccp)pok, p), "");		
	      }
	      break;
	    case '<':
	      {
		/* SREFS */
		char sok[strlen(fref)+strlen(tref)+3];
		sprintf(sok, "%s->%s", tref, fref);
		hash_add(sources_ok, pool_copy((uccp)sok, p), "");		
	      }
	      break;
	    case '+':
	      fprintf(stderr, "scolinks: pp_ref: don't know how to handle '++'\n");
	      break;
	    }
	  hash_add(seen, (uccp)pending, "");
	}
    }
}

void
pp_src(const char **atts)
{
  const char *text = (ccp)pool_copy((uccp)findAttr(atts, "qid"), p);
  if (text && *text)
    {
      const char *prj = NULL;
      const char *pqx = NULL;
      qid_prj_pqx(text, &prj, &pqx);

      char pending[strlen(prj)+strlen(pqx)+strlen(text)+3];
      sprintf(pending, "%s:%s=%s", prj, pqx, text);
      hash_add(seen, pool_copy((uccp)pending, p), "");

      char sref[strlen(prj)+strlen(pqx)+2];
      sprintf(sref, "%s:%s", prj, pqx);
      hash_add(seen, pool_copy((uccp)sref, p), (void*)text);

      char sok[strlen(text)+strlen(sref)+3];
      sprintf(sok, "%s->%s", text, sref);
      hash_add(sources_ok, pool_copy((uccp)sok, p), "");
    }
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
  else if ('l' == *name)
    {
      if (!name[1] || !strcmp(name, "lg"))
	last_lid = (char*)pool_copy((uccp)get_xml_id(atts), p);
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

  badatf = hash_create(128);
  defs = hash_create(128);
  indexed = hash_create(128);
  linkindex = hash_create(1024);
  parallels_ok = hash_create(128);
  prefs = hash_create(128);
  seen = hash_create(128);
  sources_ok = hash_create(128);
  srefs = hash_create(128);
  symdefs = hash_create(128);
  p = pool_init();

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
