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

  lp = loadfile_lines3((uccp)merge_fn(sl, aslconfig.merge), &nline, &fmem);
  int i;
  for (i = 0; i < nline; ++i)
    {
      if ('o' == lp[i][0]) /* ignore o0000237++ o0000237 o0002827 lines */
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
	      /* Now v points to the mkey, e.g., o0000237++ */
	      /*hash_add(sl->h_merges, lp[i], v);*/
	      char *vv = strdup((ccp)v);
	      char **mm = space_split(vv);
	      /* Now mm are all the OIDs that contribute to the mkey, both head and mergees */
	      int j;
	      for (j = 0; mm[j]; ++j)
		hash_add(mhash, v, mm[j]);
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
  const char *key = hash_find(mhash, t);
  if (key)
    printf("%s\t%s\t%s\n", key, vido_new_id(vp,key), qid);
}

int
main(int argc, char **argv)
{
  options(argc,argv,"d:np:");
  char buf[1024], *b, qid[1024], wdid[32];
  Vido *vp = vido_init('t', 0);
  if (project)
    {
      mpool = pool_create();
      mhash = hash_init(100);
      asl_config(project);
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
	  printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);

	  /* if the key contains ..[^\000] the SFV is o0..v so just zero dot[2] and save the sign key */
	  char *dot = strchr(t, '.');
	  if ('.' == dot[1])
	    {
	      if (dot[2])
		{
		  dot[2] = '\0';
		  printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);
		}
	      /* This handles o0000087.. and o0000087..a */
	      merge_entry(t,vp,qid);
	    }
	  else
	    {
	      /* we have to save S.F. */
	      dot = strrchr(t, '.');
	      if (dot[2])
		{
		  dot[1] = '\0';
		  printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);
		}
	      merge_entry(t,vp,qid);
	    }
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
help ()
{
  printf("  -d [index_dir] Gives the name of the index directory; defaults to 02pub/tok\n");
  printf("  -n no output; suppress vid dump\n");
  printf("  -p project; required if merge data is used by project\n");
}
