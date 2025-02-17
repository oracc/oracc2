#include <oraccsys.h>

typedef struct M
{
  char *p;
  char *k;
  char *t;
  char *c;
} M;

static void
mpsplit(M *mp, char *s)
{
  char *c = strchr(s,':');
  if (c)
    {
      mp->p = s;
      *c++ = '\0';
      mp->k = c;
    }
  else
    {
      mp->k = s;
    }
  mp->t = strchr(mp->k, '\t');
  *mp->t++ = '\0';
  mp->c = strchr(mp->t, '\t');
  *mp->c++ = '\0';  
}

int
main(int argc, char **argv)
{
  size_t nline;
  unsigned char *fmem;
  char **lp = (char**)loadfile_lines3((uccp)"-", &nline, &fmem);
  Hash *o = hash_create(1024);
  Hash *p = hash_create(10);
  Memo *m = memo_init(sizeof(M),8192);
  int i;
  for (i = 0; lp[i]; ++i)
    {
      M* mp = memo_new(m);
      mpsplit(mp, lp[i]);
      if (!mp->p)
	hash_add(o, (uccp)mp->k, mp);
      else
	{
	  Hash *ph = hash_find(p, (uccp)mp->p);
	  if (!ph)
	    {
	      ph = hash_create(1024);
	      hash_add(p, (uccp)mp->p, ph);
	    }
	  hash_add(ph, (uccp)mp->k, mp);
	}
    }
  int nok;
  char **ok = (char**)hash_keys2(o, &nok);
  FILE *fp = stdout;
  fputs("<csltab>", fp);
  for (i = 0; ok[i]; ++i)
    {
      M *mp = hash_find(o, (uccp)ok[i]);
      fprintf(fp, "<o k=\"%s\" c=\"%s\" t=\"%s\">", mp->k, mp->c, mp->t);
      
      fputs("</o>", fp);
    }
  fputs("</csltab>", fp);
}
