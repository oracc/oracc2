#include <oraccsys.h>
#include <hash.h>
#include <list.h>
#include <pool.h>
#include <form.h>

/* toksigs: read argv[1]=.tok file write the signatures in
 * tab-delimited format
 */

Pool *p;
char *curr_labl = 0;
char *curr_atfl = 0;
char *curr_atff = 0;
int conllo = 0;
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
      fprintf(sigsfp, "%s\t%s\t%s\t%s\t%s", curr_atff, curr_atfl, inst, f2.project, wid);
      form_serialize_tab(sigsfp, &f2);
      fputc('\n', sigsfp);
    }
}

int
main(int argc, char *const *argv)
{
  sigsfp = stdout;
  tokfn = argv[1];
  int xcl = 0;
  p = pool_init();
  FILE *tokfp = xfopen(tokfn, "r");
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
}
