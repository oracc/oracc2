#include <oraccsys.h>
#include <roco.h>
#include <runexpat.h>

const char *arg_project = NULL;
const char *current_proj = NULL;
const char *current_pqid = NULL;
const char *curr_text = NULL;
char *last_lid = NULL;
const char *output_fn = NULL;
const char *qid_fn = NULL;
FILE *outfp = NULL;
int auto_ex = 0;
int in_lg = 0;
int stdin_input = 0;

Hash *badatf = NULL;
Hash *defs = NULL;
Hash *indexed = NULL;
Hash *linkindex = NULL;
Hash *pending = NULL;
Hash *prefs = NULL;
Hash *parallels_ok = NULL;
Hash *seen = NULL;
Hash *sources_ok = NULL;
Hash *srefs = NULL;
Hash *symdefs = NULL;
Pool *p = NULL;

void
hash_sort_exec(Hash *h, hash_exec_func fnc)
{
  int nhk;
  const char **hk = hash_keys2(h, &nhk);
  qsort(hk,nhk,sizeof(const char *), cmpstringp);
  int i;
  for (i = 0; i < nhk; ++i)
    fnc(hash_find(h, (uccp)hk[i]));
}

void
hash_sort_exec2(Hash *h, hash_exec_func fnc)
{
  int nhk;
  const char **hk = hash_keys2(h, &nhk);
  qsort(hk, nhk, sizeof(const char *), cmpstringp);

}

void
hash_sort_user_key(Hash *h, hash_exec_func fnc, void *k)
{
  int nhk;
  const char **hk = hash_keys2(h, &nhk);
  qsort(hk,nhk,sizeof(const char *), cmpstringp);

}

void
hash_hash_add(Hash *h, const char *k, const char *kk)
{
  Hash *hh = hash_find(h, (uccp)k);
  if (!hh)
    hash_add(h, (uccp)k, (hh = hash_create(128)));
  hash_add(hh, (uccp)kk, "");
}

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
      char symkey[strlen(curr_text)+strlen(pqx)+3];
      sprintf(symkey, "%s:%s", curr_text, pqx);
      hash_add(symdefs, pool_copy((uccp)symkey, p), (void*)sym);
      if (!strcmp(curr_text, text))
	fprintf(stderr, "scolinks: text %s refers to itself in #link: def\n", curr_text);
      else
	{
	  char pend[strlen(curr_text)+strlen(text)+2];
	  sprintf(pend, "%s=%s", curr_text, text);
	  hash_add(pending, pool_copy((uccp)pend, p), "");
	}
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

      char pending[strlen(curr_text)+strlen(text)+3];
      sprintf(pending, "%s=%s", curr_text, text);
      hash_add(seen, pool_copy((uccp)pending, p), "");
      sprintf(pending, "%s=%s", text, curr_text);
      hash_add(seen, pool_copy((uccp)pending, p), "");

      char pref[strlen(prj)+strlen(pqx)+2], *prefp;
      sprintf(pref, "%s:%s", prj, pqx);
      prefp = (char*)pool_copy((uccp)pref, p);
      hash_add(seen, (uccp)prefp, (void*)text);
      hash_add(seen, (uccp)text, (void*)prefp);

      hash_hash_add(prefs, text, prefp);
      hash_hash_add(prefs, prefp, text);

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
  if (symprj) /* =~ if ($defs{$sym}) in harvest-links.plx */
    {
#if 0
      /* This was defined in harvest-links.plx but the resultant flag
	 string was never used */
      char *flags = "";
      if ('?' == label[strlen(label)-1])
	{
	  flags = "?";
	  char *end = (char*)(label+strlen(label));
	  *--end = '\0';
	  while (end > label && isspace(end[-1]))
	    *--end = '\0';
	}
#endif

      char xlink[strlen(symprj)+strlen(sympqx)+strlen(label)+3];
      sprintf(xlink, "%s:%s~%s", symprj, sympqx, label);

      const char *fref = curr_text;

      char tref_buf[strlen(symprj)+strlen(sympqx)+2], *tref;
      sprintf(tref_buf, "%s:%s", symprj, sympqx);
      tref = (char*)pool_copy((uccp)tref_buf, p);

      const char *xlx = NULL;
      
      if ((xlx = hash_find(linkindex, (uccp)xlink)))
	{
	  const char *lid = last_lid;
	  if ('<' == type)
	    {
	      type = '>';
	      symdef = tref;
	      const char *tmp = fref;
	      fref = tref;
	      tref = (char*)tmp;
	      tmp = lid;
	      lid = xlx;
	      xlx = tmp;	      
	    }
	  /* harvest-links.plx did $xlx/$lid =~ s,^.*?/,, but why? */

	  /* for rel we factored out '<' above */
	  fprintf(outfp, "<link rel=\"%s\">", type=='>'?"goesto":"parallels");
	  fprintf(outfp, "<from ref=\"%s\" line=\"%s\"/>", fref, lid);
	  fprintf(outfp, "<to ref=\"%s\" line=\"%s\"/>", tref, xlx);
	  fputs("</link>", outfp);

	  if ('>' == type)
	    {
		char pend[strlen(symdef)+strlen(fref)+2];
		sprintf(pend, "%s=%s\n", symdef, fref);
		hash_add(seen, pool_copy((uccp)pend, p), "");
		hash_hash_add(srefs, tref, fref);

		char sok[strlen(fref)+strlen(tref)+3];
		sprintf(sok, "%s->%s", fref, tref);
		hash_add(sources_ok, pool_copy((uccp)sok, p), "");
	    }
	  else if ('+' == type)
	    {
	      /* unhandled case for now */
	    }
	  else
	    {
		char pend[strlen(symdef)+strlen(fref)+2];
		sprintf(pend, "%s=%s\n", fref, symdef);
		hash_add(seen, pool_copy((uccp)pend, p), "");

		sprintf(pend, "%s=%s\n", symdef, fref);
		hash_add(seen, pool_copy((uccp)pend, p), "");

		hash_hash_add(prefs, symdef, fref);
		hash_hash_add(prefs, fref, symdef);

		char pok[strlen(fref)+strlen(tref)+3];
		sprintf(pok, "%s->%s", fref, tref);
		hash_add(parallels_ok, pool_copy((uccp)pok, p), "");		
	    }
	}
      else
	{
	  const char *pending = NULL;
	  const char *symdef = hash_find(defs, (uccp)sig);
	  switch (type)
	    {
	    case '>':
	      {
		char pend[strlen(symdef)+strlen(fref)+2];
		sprintf(pend, "%s=%s\n", symdef, fref);
		pending = (ccp)pool_copy((uccp)pend, p);

		hash_hash_add(srefs, symdef, fref);
		char sok[strlen(fref)+strlen(tref)+3];
		sprintf(sok, "%s->%s", fref, tref);
		hash_add(sources_ok, pool_copy((uccp)sok, p), "");
	      }
	      break;
	    case '|':
	      {
		char pend[strlen(symdef)+strlen(fref)+2];
		sprintf(pend, "%s=%s\n", fref, symdef);
		pending = (ccp)pool_copy((uccp)pend, p);
		hash_add(seen, (uccp)pending, "");

		sprintf(pend, "%s=%s\n", symdef, fref);
		pending = (ccp)pool_copy((uccp)pend, p);

		hash_hash_add(prefs, symdef, fref);
		hash_hash_add(prefs, fref, symdef); 

		char pok[strlen(fref)+strlen(tref)+3];
		sprintf(pok, "%s->%s", fref, tref);
		hash_add(parallels_ok, pool_copy((uccp)pok, p), "");		
	      }
	      break;
	    case '<':
	      {
		char pend[strlen(symdef)+strlen(fref)+2];
		sprintf(pend, "%s=%s\n", fref, symdef);
		pending = (ccp)pool_copy((uccp)pend, p);

		hash_hash_add(srefs, fref, symdef);
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

      char sref[strlen(prj)+strlen(pqx)+2], *srefp;
      sprintf(sref, "%s:%s", prj, pqx);
      hash_add(seen, (uccp)(srefp = (char*)pool_copy((uccp)sref, p)), (void*)text);

      hash_hash_add(srefs, srefp, text);

      char sok[strlen(text)+strlen(sref)+3];
      sprintf(sok, "%s->%s", text, sref);
      hash_add(sources_ok, pool_copy((uccp)sok, p), "");
    }
}

void
pp_pending(void)
{
  const char **k = hash_keys(pending);
  int i;
  for (i = 0; k[i]; ++i)
    {
      if (!hash_find(seen, (uccp)k[i]))
	{
	  char *to = memo_dup(k[i]);
	  char *from = strchr(to, '=');
	  *from++ = '\0';
	  hash_hash_add(srefs, to, from);
	}
    }
}

void
pp_sref_r(const char *r, const char *k)
{
  char *xsym = strchr(r,':')+1;
  char symbuf[strlen(xsym)+strlen(current_proj)+2];
  sprintf(symbuf, "%s:%s", current_proj, xsym);
  char *sym = (char*)pool_copy((uccp)symbuf, p);

  char *exsym1 = memo_dup(sym);
  char *exsym2 = strchr(exsym1,':');
  *exsym2++ = '\0';
  char *exk = strchr(k,':')+1;
  char exsym[strlen(exsym1)+strlen(exk)+strlen(exsym2)+3];
  sprintf(exsym, "%s:%s:%s", exsym1, exk, exsym2);

  char *ex = hash_find(symdefs, (uccp)exsym);
  if (!ex)
    {
      char exx[7];
      sprintf(exx, "EX%03d", ++auto_ex);
      ex = exx;
    }
  char buf[strlen(ex)+strlen(" sig=''0")];
  sprintf(buf, " sig=\"%s\"", ex);
  char *sigattr = (char*)pool_copy((uccp)buf, p);

  char sok[strlen(k)+strlen(r)+3];
  sprintf(sok, "%s->%s", k, r);
  char *linksno = hash_find(sources_ok, (uccp)sok) ? "" : " links=\"no\"";

  fprintf(outfp, "<r ref=\"%s\"%s%s/>", r, sigattr, linksno);
}

void
pp_srefs_one(const char *k, Hash *h)
{
  fprintf(outfp, "<refs type=\"sources\" ref=\"%s\">", k);
  auto_ex = 0;
  hash_sort_user_key(h, (hash_exec_func*)pp_sref_r, (void*)k);
  fputs("</refs>", outfp);
}

void
pp_srefs(void)
{
  hash_sort_exec2(srefs, (hash_exec_func*)pp_srefs_one);
}

void
pp_pref_r(const char *r)
{
  fprintf(outfp, "<r ref=\"%s\"/>", r);
}

void
pp_prefs_one(const char *k, Hash *h)
{
  fprintf(outfp, "<refs type=\"parallel\" ref=\"%s\">", k);
  hash_sort_exec(h, (hash_exec_func*)pp_pref_r);
  fputs("</refs>", outfp);
}

void
pp_prefs(void)
{
  hash_sort_exec2(prefs, (hash_exec_func*)pp_prefs_one);
}

/* unlike harvest-links.plx this only handles the post-fileset-outputs */
void
process_project(void)
{
  pp_pending();
  pp_srefs();
  pp_prefs();
}

void
pp_xml_begin(void)
{
  fputs("<linkbase>", outfp);
  if (arg_project)
    fprintf(outfp, "<project n=\"%s\">", arg_project);
}

void
pp_xml_end(void)
{
  process_project();
  if (arg_project)
    fputs("</project>", outfp);
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
process_incref(const char *name, const char **atts)
{
  const char *ref = findAttr(atts, "ref");
  if ('i' == *name)
    {
      char pending[strlen(curr_text)+strlen(ref)+2];
      sprintf(pending, "%s=%s", curr_text, ref);
      hash_add(seen, pool_copy((uccp)pending, p), "");

      hash_hash_add(srefs, curr_text, ref);

      char sok[strlen(curr_text)+strlen(ref)+2];
      sprintf(sok, "%s->%s", ref, curr_text);
      hash_add(sources_ok, pool_copy((uccp)sok, p), "");
    }
  else
    {
      char pending[strlen(curr_text)+strlen(ref)+2];
      sprintf(pending, "%s=%s", curr_text, ref);
      hash_add(seen, pool_copy((uccp)pending, p), "");

      sprintf(pending, "%s=%s", ref, curr_text);
      hash_add(seen, pool_copy((uccp)pending, p), "");

      hash_hash_add(prefs, curr_text, ref);
      hash_hash_add(prefs, ref, curr_text);
      
      char pok[strlen(curr_text)+strlen(ref)+2];
      sprintf(pok, "%s->%s", ref, curr_text);
      hash_add(parallels_ok, pool_copy((uccp)pok, p), "");
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
      if ((!name[1] && !in_lg) || !strcmp(name, "lg"))
	{
	  last_lid = (char*)pool_copy((uccp)get_xml_id(atts), p);
	  if (name[1])
	    in_lg = 1;
	}
    }
  else if (('i' == *name && !strcmp(name, "include"))
	   || ('r' == *name && !strcmp(name, "referto")))
    process_incref(name, atts);
}

void
eH(void *userData, const char *name)
{
  if (!strcmp(name, "lg"))
    in_lg = 0;
}

void
set_curr_text(void)
{
  char buf[strlen(current_proj)+strlen(current_pqid)+2];
  sprintf(buf, "%s:%s", current_proj, current_pqid);
  curr_text = memo_dup(buf);
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
      set_curr_text();
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
  FILE *infp;
  char PQ[512];
  const char *fname[2] = { NULL, NULL };
  options(argc, argv, "i:o:p:q:s");
  if (output_fn)
    outfp = freopen(output_fn, "w", stdout);
  else
    outfp = stdout;
  if (!outfp)
    {
      fprintf(stderr, "scolinks: unable to write to %s. Stop.\n", output_fn);
      exit(1);
    }

  lmemo_init();
  badatf = hash_create(128);
  defs = hash_create(128);
  indexed = hash_create(128);
  linkindex = hash_create(1024);
  parallels_ok = hash_create(128);
  pending = hash_create(128);
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
      set_curr_text();
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
      if (qid_fn)
	{
	  if (!(infp = freopen(qid_fn, "r", stdin)))
	    {
	      fprintf(stderr, "scolinks: unable to read qid file %s. Stop.\n", qid_fn);
	      exit(1);
	    }
	}
      else
	infp = stdin;
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
    case 'p': arg_project = current_proj = str; break;
    case 'q': qid_fn = str; break;
    case 's': stdin_input = 1; break;
    default: return 1; break;
    }
  return 0;
}
