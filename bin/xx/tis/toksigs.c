#include <oraccsys.h>
#include <hash.h>
#include <list.h>
#include <pool.h>
#include <form.h>

/* toksigs: read argv[1]=.tok file write the signatures in
 * tab-delimited format
 */

int sentence_boundaries = 0;

Hash *hinst;
Pool *p;
char *curr_labl = NULL;
char *curr_atfl = NULL, *last_atfl = "";
char *curr_atff = NULL;
int conllo = 0, toks_stdin;
const char *tokfn = NULL;
FILE *sigsfp = NULL;

int
Y_xcl(unsigned char *s)
{
  while (*s && !isspace(*s))
    ++s;
  while (isspace(*s))
    ++s;
  if (!strncmp((ccp)s, "xcl", 3))
    return 1;
  else
    return 0;
}

void
F_arg(unsigned char *s)
{
  curr_atff = strchr((ccp)s, '\t')+1;
  curr_atff = (char*)pool_copy((uccp)curr_atff, p);
  char *t = strchr(curr_atff, '\t');
  if (t)
    *t = '\0';
}

void
L_args(unsigned char *s)
{
  s = (ucp)strchr((ccp)s, '\t')+1;
  s = (ucp)strchr((ccp)s, '\t')+1;
  curr_labl = (char*)s;
  curr_atfl = (char*)strchr((ccp)s, '\t');
  *curr_atfl++ = '\0';
  curr_labl = (char*)pool_copy((uccp)curr_labl, p);
  curr_atfl = (char*)pool_copy((uccp)curr_atfl, p);
  if (strcmp(curr_atfl, last_atfl))
    {
      if (hinst)
	hash_free(hinst, NULL);
      hinst = hash_create(128);
      last_atfl = curr_atfl;
    }
}

#define form_char(fp,c,v) fprintf(fp,"\t%s%s",c,((!conllo&&v)?v:""))

void
form_serialize_tab(FILE *f_f2, Form *f)
{
  if (!conllo)
    form_char(f_f2,":",(char*)f->form);
  form_char(f_f2,"%",(char*)f->lang);
  form_char(f_f2,"",(char*)f->cf);
  form_char(f_f2,"",(char*)f->gw);
  form_char(f_f2,"",(char*)f->sense);
  form_char(f_f2,"",(char*)f->pos);
  form_char(f_f2,"",(char*)f->epos);
  form_char(f_f2,"$",(char*)f->norm);
  form_char(f_f2,"/",(char*)f->base);
  form_char(f_f2,"+",(char*)f->cont);
  form_char(f_f2,"*",(char*)f->stem);
  form_char(f_f2,"#",(char*)f->morph);
  form_char(f_f2,"##",(char*)f->morph2);
  /*form_char(f_f2,"rws",(char*)f->rws);*/
}

void
sb_out(const char *inst, int nth)
{
  fprintf(sigsfp, "%s\t%s/%d\t%s\n", curr_atff, curr_atfl, nth, inst);
}

void
sebo(Form *f, const char *inst, int nth)
{
  if (f->pos && 'V' == *f->pos)
    {      
      if (f->morph)
	{
	  if (strchr((ccp)f->morph, ':'))
	    {
	      const char *c = strchr((ccp)f->morph, ';');
	      if (c)
		{
		  int len = strlen(c);
		  if (!strcmp(&c[len-2], "am")
		      && (';' == c[len-3] || '.' == c[len-3]))
		    {
		      fprintf(stderr, "%s:%s: sentence boundary at copula %s\n",
			      curr_atff, curr_atfl, f->form);
		      sb_out(inst, nth);
		    }
		  else
		    {
		      ++c;
		      if (!strstr(c, ".a") && !strstr(c, ".*a") && !strchr(c, ','))
			sb_out(inst, nth);
		    }
		}
	      else
		{
		  if (!strncmp((ccp)f->morph, "nu:", 3))
		    fprintf(stderr, "%s:%s: sentence boundary at nu:~ form %s\n",
			    curr_atff, curr_atfl, f->form);
		  sb_out(inst, nth);
		}
	    }
	  else if (strchr((ccp)f->morph, '!'))
	    sb_out(inst, nth);
	  else
	    {
	      /* check for ",am" or ",ak.am" etc */
	      int len = strlen((ccp)f->morph);
	      if (!strcmp((ccp)&f->morph[len-2], "am")
		  && (';' == f->morph[len-3] || '.' == f->morph[len-3]))
		{
		  fprintf(stderr, "%s:%s: sentence boundary at copula %s\n",
			  curr_atff, curr_atfl, f->form);
		  sb_out(inst, nth);
		}
	    }
	}
    }
}

void
sigs_out(char *l)
{
  Form f2;
  memset(&f2,'\0',sizeof(Form));
  char *wid = strchr(l, '\t')+1;
  char *sig = strchr(wid, '\t');
  if (sig)
    *sig++ = '\0';
  char *inst = strchr(sig, '\t');
  if (inst)
    {
      *inst++ = '\0';
      char *tab = strchr(inst, '\t');
      if (tab)
	*tab = '\0';
    }
  else
    inst = "";
  if ('@' == *sig)
    {
      form_parse((uccp)"<tok>", 0, (ucp)sig, &f2, NULL);
      if (sentence_boundaries)
	{
	  int nth;
	  if (!(nth = (uintptr_t)hash_find(hinst, (uccp)inst)))
	    inst = (char*)pool_copy((uccp)inst,p);
	  hash_add(hinst, (uccp)inst, (void*)(uintptr_t)++nth);
	  sebo(&f2, inst, nth);
	}
      else
	{
	  fprintf(sigsfp, "%s\t%s\t%s\t%s\t%s", curr_atff, curr_atfl, inst, f2.project, wid);
	  form_serialize_tab(sigsfp, &f2);
	  fputc('\n', sigsfp);
	}
    }
}

int
main(int argc, char *const *argv)
{
  options(argc, argv, "sS");
  sigsfp = stdout;
  tokfn = argv[optind];
  int xcl = 0;
  p = pool_init();
  FILE *tokfp = NULL;
  if (toks_stdin)
    tokfp = stdin;
  else
    tokfp = xfopen(tokfn, "r");
  if (tokfp)
    {
      unsigned char *s;
      while ((s = loadoneline(tokfp, NULL)))
	{
	  if ('l' == *s)
	    sigs_out((char*)s);
	  else if ('L' == *s && xcl)
	    L_args(s);
	  else if ('F' == *s)
	    F_arg(s);
	  else if ('Y' == *s && Y_xcl(s))
	    xcl = 1;
	}
      xfclose(argv[1], tokfp);
    }
  if (hinst)
    hash_free(hinst, NULL);
}

int
opts(int argc, const char *optarg)
{
  switch (argc)
    {
    case 's':
      toks_stdin = 1;
      break;
    case 'S':
      sentence_boundaries = 1;
      break;
    default:
      return 1;
    }
  return 0;
}
void help(void){}
