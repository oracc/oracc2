#include <oraccsys.h>
#include <vido.h>
#include <asl.h>

/**tokex reads and expands the output of tokx, simplifying it to a
 * stream of triples of the form:
 *
 * token token-ID word-ID
 *
 * The expansions vary according to token type.
 *
 * For 'g' type, graphemes, the expansion includes generating SIGN and
 * FORM parent entries for values and various features based on the
 * grapheme signature.
 */

extern struct sl_config aslconfig;

const char *index_dir = "02pub/tok";
const char *project = NULL;
int no_output = 0;

Pool *mpool;
Hash *mhash;

Pool *fpool;
Hash *fhash = NULL;

struct sl_config aslcfg;

const char *signlist = "osl";

const char *prlists_file = NULL;
Hash *prlists = NULL;

static void
prlists_load_one(const char *base)
{
  char list[strlen("00lib/lists/0")+strlen(base)];
  sprintf(list, "00lib/lists/%s", base);
  size_t nline;
  unsigned char *fmem;
  unsigned char **lp = loadfile_lines3((uccp)list, &nline, &fmem);
  int i;
  for (i = 0; lp[i]; ++i)
    {
      char *pqx = (char*)lp[i], *tmp = strchr((ccp)lp[i],':');
      if (tmp)
	pqx = tmp+1;
      if ((tmp = strchr(pqx,'@')))
	*tmp = '\0';
      List *l = hash_find(prlists, (uccp)pqx);
      if (!l)
	{
	  l = list_create(LIST_SINGLE);
	  hash_add(prlists, (uccp)pqx, l);
	}
      list_add(l, (void*)base);
    }
}

static void
prlists_load(void)
{
  size_t nline;
  unsigned char *fmem;
  unsigned char **lp = loadfile_lines3((uccp)prlists_file, &nline, &fmem);
  int i;
  prlists = hash_create(1024);
 for (i = 0; lp[i]; ++i)
    prlists_load_one((ccp)lp[i]);
}

static char *
pqx_of(const char *qid)
{
  static char pqx[8];
  qid = strchr(qid, ':');
  ++qid;
  strncpy(pqx,qid,7);
  pqx[7] = '\0';
  return pqx;
}

static void
pr(const char *k, Vido *vp, const char *q)
{
  printf("%s\t%s\t%s\n", k, vido_new_id(vp,k), q);
  if (prlists)
    {
      List *prlist = hash_find(prlists, (uccp)pqx_of(q));
      const char *prefix;
      for (prefix = list_first(prlist); prefix; prefix = list_next(prlist))
	{
	  char pk[strlen(prefix)+strlen(k)+2];
	  sprintf(pk, "%s:%s", prefix, k);
	  printf("%s\t%s\t%s\n", pk, vido_new_id(vp,pk), q);
	}
    }
}

/* tokex operates on corpus token output so it has to use the corpus
   forms table; this needs parameterization by pcsl/osl/pesl */
static char *
forms_fn(const char *project)
{
  char buf[strlen(oracc())+strlen("//")+(2*strlen(signlist))+strlen("/02pub/.frm0")];
  sprintf(buf, "%s/%s/02pub/%s.frm", oracc(), signlist, signlist);
  return (char*)pool_copy((uchar *)buf, fpool);  
}

static void
forms_load(void)
{
  size_t nline;
  fpool = pool_init();
  const char *ffn = forms_fn(project);
  if (!access(ffn, R_OK))
    {
      fhash = hash_create(1024);
      unsigned char *fmem;
      unsigned char **lp = loadfile_lines3((uccp)ffn, &nline, &fmem);
      int i;
      for (i = 0; i < nline; ++i)
	{
	  char buf[strlen((ccp)lp[i])+3];
	  sprintf(buf, "%s+f", lp[i]);
	  hash_add(fhash, (uccp)pool_copy((uccp)lp[i], fpool), pool_copy((uccp)buf, fpool));
	}
    }
}

void
forms_entry(char *t, Vido *vp, char *qid)
{
  char *dot = strchr(t, '.');
  if (dot)
    *dot = '\0';
  const char *key = hash_find(fhash, (uccp)t);
  if (key)
    /*printf("%s\t%s\t%s\n", key, vido_new_id(vp,key), qid);*/
    pr(key, vp, qid);
}

char *
merge_fn(const char *script)
{
  char buf[strlen(oracc())+strlen("/osl/")+strlen(script)+strlen("/02pub/.mrg0")];
  sprintf(buf, "%s/osl/02pub/%s.mrg", oracc(), script);
  return (char*)pool_copy((uchar *)buf, mpool);
}

static void
merge_load(void)
{
  size_t nline;
  unsigned char *fmem;
  unsigned char **lp = NULL;
  char *mfn = merge_fn(aslcfg.merge);

  /* silently ignore missing .mrg files */
  if (access(mfn, R_OK))
    return;
  
  lp = loadfile_lines3((uccp)mfn, &nline, &fmem);
  int i;
  for (i = 0; i < nline; ++i)
    {
      if ('o' != lp[i][0]) /* ignore non- o0000237+m o0000237 o0002827 lines */
	continue;
      unsigned char *v = lp[i];
      while (*v && !isspace(*v))
	++v;
      if (*v)
	{
	  *v++ = '\0';
	  while (isspace(*v))
	    ++v;
	  if (*v)
	    {
	      /* Now lp[i] points to the mkey, e.g., o0000237++ */
	      char *vv = strdup((ccp)v);
	      char **mm = space_split(vv);
	      /* Now mm are all the OIDs that contribute to the mkey, both head and mergees */
	      int j;
	      for (j = 0; mm[j]; ++j)
		hash_add(mhash, (uccp)mm[j], lp[i]);
	    }
	}
    }
}

void
merge_entry(char *t, Vido *vp, char *qid)
{
  char *dot = strchr(t, '.');
  if (dot)
    *dot = '\0';
  const char *key = hash_find(mhash, (uccp)t);
  if (key)
    /*printf("%s\t%s\t%s\n", key, vido_new_id(vp,key), qid);*/
    pr(key, vp, qid);
}

int
main(int argc, char **argv)
{
  options(argc,argv,"d:nl:p:");
  char buf[1024], *b, qid[1024], wdid[32];
  Vido *vp = vido_init('t', 0);
  fpool = pool_init();
  forms_load();
  if (project)
    {
      mpool = pool_init();
      mhash = hash_create(100);
      asl_config(project, &aslcfg);
      if (aslcfg.merge)
	merge_load();
    }
  while ((b = fgets(buf, 1024, stdin)))
    {
      if (b[strlen(b)-1] == '\n')
	b[strlen(b)-1] = '\0';

      if ('W' == *b)
	strcpy(wdid, 1+strchr(b, '\t'));
      else if ('g' == *b)
	{
	  char *t, *s, *w;
	  t = strchr(b, '\t');
	  if (t)		/* token */
	    *t++ = '\0';
	  if ('.' == *t)
	    continue;	  
	  s = strchr(t, '\t');  /* signature */
	  if (s)
	    *s++ = '\0';
	  if ('@' == *s)
	    {
	      ++s;
	      char *t = strchr(s, '%');
	      *t = '\0';
	      strcpy(qid, s);
	      strcat(qid, ":");
	      strcat(qid, wdid);
	    }
	  w = strchr(s, '\t');
	  if (w)
	    *w++ = '\0';

	  /* Always save the entire S.F.V */
	  /*printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);*/
	  pr(t, vp, qid);

	  /* if the key contains ..[^\000] the SFV is o0..v so just zero dot[2] and save the sign key */
	  char *dot = strchr(t, '.');
	  if ('.' == dot[1])
	    {
	      if (dot[2])
		{
		  dot[2] = '\0';
		  /*printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);*/
		  pr(t, vp, qid);
		}
	      /* This handles o0000087.. and o0000087..a */
	      merge_entry(t,vp,qid);
	    }
	  else
	    {
	      /* we have to save S.F. */
	      dot = strrchr(t, '.');
	      if (dot[1]) /* . is the . before value; if dot[1] is not \0 there is a value */
		{
		  dot[1] = '\0';
		  /*printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);*/
		  pr(t, vp, qid);
		}
	      merge_entry(t,vp,qid);
	    }
	  if (fhash)
	    forms_entry(t,vp,qid);
	}
    }
  if (!no_output)
    {
      char vidfile[strlen(index_dir)+strlen("tid.vid0")];
      char tsvfile[strlen(index_dir)+strlen("tid.vid0")];
      sprintf(vidfile,"%s/tid.vid",index_dir);
      sprintf(tsvfile,"%s/tid.tsv",index_dir);
      vido_dump_data(vp, vidfile, tsvfile);
      vido_term(vp);
    }
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'd':
      index_dir = arg;
      break;
    case 'l':
      prlists_file = arg;
      prlists_load();
      break;
    case 'n':
      no_output = 1;
      break;
    case 'p':
      project = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "tokex";
int major_version = 6, minor_version = 0, verbose;
const char *usage_string = "-d [index_dir] [-n]";
void
help (void)
{
  printf("  -d [index_dir] Gives the name of the index directory; defaults to 02pub/tok\n");
  printf("  -l [lists_file] give name of list of lists for tokex slices\n");
  printf("  -n no output; suppress vid dump\n");
  printf("  -p project; required if merge data is used by project\n");
}
