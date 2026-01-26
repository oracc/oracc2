#include <oraccsys.h>
#include <pool.h>

Hash *v;

enum frmfld { FF_FORM, FF_NORM, FF_BASE, FF_STEM,
	      FF_CONT, FF_MRF1, FF_MRF2, FF_PERD,
	      FF_TOP };

const char *frmchr = "=$/*+#&@";

int frmflds[128];

void
ff_init(void)
{
  frmflds['=']=FF_FORM;
  frmflds['$']=FF_NORM;
  frmflds['/']=FF_BASE;
  frmflds['*']=FF_STEM;
  frmflds['+']=FF_CONT;
  frmflds['#']=FF_MRF1;
  frmflds['&']=FF_MRF2;
  frmflds['@']=FF_PERD;
}

typedef struct val
{
  uccp v;
  struct val *next;
} Val;

typedef struct val *Vals[FF_TOP];

Memo *mem_vals;
Memo *mem_valss;
Pool *p;

int
main(int argc, char * const *argv)
{
  /*size_t line = 0;*/
  const char *file;
  FILE *in_fp;
  ff_init();
  p = hpool_init();
  mem_vals = memo_init(sizeof(Val), 1024);
  mem_valss = memo_init(sizeof(Vals), 256);
  v = hash_create(1024);
  if (argv[optind])
    {
      file = argv[optind];
      in_fp = xfopen(file, "r");
      if (!in_fp)
	exit(1);
    }
  else
    {
      file = "-";
      in_fp = stdin;
    }

  unsigned char *sem, *bis, *ter, *tab;
  while ((sem = loadoneline(in_fp, NULL)))
    {
      /*++line;*/
      if (!(bis = (ucp)strchr((ccp)sem, (char)1)))
	continue;
      *bis++ = '\0';
      if (bis[0] != (char)1)
	continue;
      ter = (ucp)strchr((ccp)bis, (char)1);
      *ter++ = '\0';

      /* if we are looking at a tab this is the CGP or CGPSE */
      if ('\t' == ter[0])
	continue;

      tab = (ucp)strchr((ccp)ter, '\t');
      if (tab)
	*tab = '\0';
      Vals *vals = hash_find(v, sem);
      if (!vals)
	{
	  vals = memo_new(mem_valss);
	  hash_add(v, pool_copy(sem, p), vals);
	}
      enum frmfld fi;
      if (isdigit(ter[0]))
	{
	  fi = FF_PERD;
	}
      else if ('#' == ter[0] && '#' == ter[1])
	{
	  fi = FF_MRF2;
	  ter += 2;
	}
      else
	{
	  fi = frmflds[*ter++];
	}
      Val *val = memo_new(mem_vals);
      val->v = pool_copy(ter, p);
      if (vals[fi])
	val->next = (*vals)[fi];
      (*vals)[fi] = val;
    }

  const char **k = hash_keys(v);
  size_t i;
  for (i = 0; k[i]; ++i)
    {
      Vals *vp = hash_find(v, (uccp)k[i]);
      int j;
      for (j = 0; j < FF_TOP; ++j)
	{
	  if ((*vp)[j])
	    {
	      Val *vvp = (*vp)[j];
	      printf("%s\t%c\t", k[i], frmchr[j]);
	      while (vvp)
		{
		  fputs((ccp)vvp->v, stdout);
		  if (vvp->next)
		    putchar(' ');
		  vvp = vvp->next;
		}
	      putchar('\n');
	    }
	}
    }
}
