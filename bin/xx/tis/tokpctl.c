#include <oraccsys.h>
#include <roco.h>
#include <math.h>

/* The following index signatures are handled with this structure:
 *
 * @sig [no ^A ]: ent=sns=NULL; dat = sig
 * ent^A^A:	  ent=sem; sns=dat=NULL
 * ent^A^Adat:	  ent=sem; sns=NULL; dat=dat
 * ent^Asns^A:	  ent=NULL; sns=ent+^A+sns; dat=NULL
 * ent^Asns^Adat: ent=NULL; sns=ent+^A+sns; dat=dat
 *
 * Note that sns is stored with its parent ent to make hashing and
 * counting easier, so one or both of ent and sns is always NULL.
 */
typedef struct sembister
{
  const unsigned char *ent;
  const unsigned char *sns;
  const unsigned char *dat;
} Triple;

static Triple* ctrl_a_split(Triple *tp, char *s);
static double pct(double amount, double total);
static double print_pct(size_t total, int amount);
static void print_row(char **row);

Hash *wcounts, *scounts;
size_t sig_count;
FILE *out_fp = NULL;
int verbose = 0;

/* Input from tokix is in three columns: token<TAB>ID<TAB>COUNT
 *
 * Tokens are divided with Ctrl-A (0x01) into three parts: sem(el),
 * bis and ter.
 *
 * For l tokens, sem = entry = CGP; bis = sense = //S'E; ter =
 * data--base,cont,morph,etc.
 *
 * For sem/entry the parent count is the total sem, i.e., words in the
 * dataset.
 *
 * For bis/sense the parent count is the count of the token's sem,
 * i.e, the percent of this word's instances that have this sense.
 *
 * For data, we calculate the percentage of word instances if bis is
 * NULL, or of sense instances if bis is non-NULL.
 */
int
main(int argc, char *const *argv)
{
  const char *file;
  FILE *in_fp;
  out_fp = stdout;

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

  wcounts = hash_create(1024);
  scounts = hash_create(2048);
  
  Roco *r = roco_load1(file);
  Triple *t = calloc(r->nlines, sizeof(Triple));
  int i;
  for (i = 0; i < r->nlines; ++i)
    {
      /* replace column one's char* with a Triple* */
      r->rows[i][0] = (void*)ctrl_a_split(&t[i], (char*)r->rows[i][0]);
      r->rows[i][2] = (void*)(uintptr_t)atoi((ccp)r->rows[i][2]);
      if (t[i].ent && !t[i].dat)
	hash_acc(wcounts, t[i].ent, (uintptr_t)r->rows[i][2]);
      else if (t[i].sns && !t[i].dat)
	hash_acc(scounts, t[i].sns, (uintptr_t)r->rows[i][2]);
      else
	{
#if 0
	  /* we don't calculate percentages for any row where dat is non-NULL */
	  if (!t[i].ent && !t[i].sns)
	    ++sig_count/* += (uintptr_t)r->rows[i][2]*/;
#endif
	}
    }

  for (int i = 0; i < r->nlines; ++i)
    print_row((char**)r->rows[i]);
}

static Triple *
ctrl_a_split(Triple *tp, char *s)
{
  if (!strchr(s, 0x01))
    tp->dat = (uccp)s;
  else
    {
      tp->ent = (uccp)s;
      s = strchr(s, 0x01);
      if ('/' == s[1])
	{
	  /* tp->sns is the whole of sem+ctrl_a+bis */
	  tp->sns = tp->ent;
	  tp->ent = NULL;
	  s = strchr(s+1, 0x01);
	  if (*s)
	    {
	      *s++ = '\0';
	      if (*s)
		tp->dat = (uccp)s;
	    }
	}
      else
	{
	  *s++ = '\0';
	  if (s[1]) /* s is looking at the ^A which ends the empty sense */
	    tp->dat = (uccp)s+1;
	}
    }
  return tp;
}

static double
print_pct(size_t total, int amount)
{
  double p = 0.0;
  if (total)
    {
      p = pct(amount, total);
      if (p == (unsigned int)p)
	fprintf(out_fp, "\t%u", (int)p);
      else
	fprintf(out_fp, "\t%.02f", p);
    }
  else
    fputs("\t_", out_fp);
  return p;
}

static double
pct(double amount, double total)
{
  double pct = (amount / total) * 100;
  if (pct == INFINITY)
    {
      if (verbose)
	fprintf(stderr, "tokpct: amount %g / total %g * 100 yields INFINITY\n", amount, total);
      pct = (double)0.0;
    }
  return pct;
}

static void
print_row(char **row)
{
#define rt_ent() (((Triple*)row[0])->ent)
#define rt_sns() (((Triple*)row[0])->sns)
#define rt_dat() (((Triple*)row[0])->dat)
  size_t total;
#if 0
  int type;
#endif
  
  if (rt_ent())
    {
#if 0
      type = 1;
#endif
      if (rt_dat())
	{
	  fprintf(out_fp, "%s\x1\x1%s\t%s\t%ld", rt_ent(), rt_dat(), row[1], (uintptr_t)row[2]);
	}
      else
	{
	  fprintf(out_fp, "%s\x1\x1\t%s\t%ld", rt_ent(), row[1], (uintptr_t)row[2]);
	}
      total = (uintptr_t)hash_find(wcounts, (uccp)rt_ent());
    }
  else if (rt_sns())
    {
#if 0
      type = 2;
#endif
      if (rt_dat())
	{
	  fprintf(out_fp, "%s\x1%s\t%s\t%ld", rt_sns(), rt_dat(), row[1], (uintptr_t)row[2]);
	}
      else
	{
	  fprintf(out_fp, "%s\x1\t%s\t%ld", rt_sns(), row[1], (uintptr_t)row[2]);
	}
      total = (uintptr_t)hash_find(scounts, (uccp)rt_sns());
    }
  else
    {
#if 0
      type = 3;
#endif
      fprintf(out_fp, "%s\t%s\t%ld", rt_dat(), row[1], (uintptr_t)row[2]);
      total = 0;
    }
#if 1
  (void)print_pct(total,(uintptr_t)row[2]);
#else
  double p = print_pct(total,(uintptr_t)row[2]);
  if (type == 1 || type == 2)
    fprintf(stderr, "print_pct: %s %s %ccounts total=%ld; row[2] = %ld; %c = %.02g\n",
	    type==1?"entry":"sense",
	    type==1?rt_ent():rt_sns(),
	    type==1?'w':'s',
	    total,
	    (uintptr_t)row[2],
	    '%',
	    p);
#endif
  fputc('\n', out_fp);
}

void help(void){}
int opts(int optch, const char*optarg){return 0;}
