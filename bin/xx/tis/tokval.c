#include <oraccsys.h>

Hash *v;

enum frmfld { FF_FORM, FF_NORM, FF_BASE, FF_STEM, FF_CONT,
	      FF_MRF1, FF_MRF2, FF_PERD, FF_TOP };

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

typedef struct val* Vals[8];

Memo *mem_vals;
Memo *mem_valss;
Pool *p;

int
main(int argc, char * const *argv)
{
  const char *file;
  FILE *in_fp;
  ff_init();
  mem_vals = memo_init(sizeof(Vals), 1024);
  mem_valss = memo_init(sizeof(Vals), 256);
  v = hash_create(8192);
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

  unsigned char *sem, *bis, *ter;  
  while ((sem = loadoneline(in_fp, NULL)))
    {
      bis = (ucp)strchr((ccp)sem, (char)1);
      *bis++ = '\0';
      ter = (ucp)strchr((ccp)bis, (char)1);
      *ter++ = '\0';
      Vals *vals = hash_find(v, sem);
      if (!vals)
	{
	  vals = memo_new(mem_valss);
	  hash_add(v, sem, vals);
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
      val->next = *vals[fi];
      *vals[fi] = val;
    }
}
