#include <oraccsys.h>
#include <hash.h>
#include <list.h>
#include <pool.h>
#include <form.h>

/* atfsig: read argv[1]=.tok file and argv[2]=.atf file and merge the
 * signatures from the tok file into the ATF.
 */

Hash *L;
Pool *p;
int curr_line = 0;
int L_max = 0;
int oconll = 0;
const char *tokfn = NULL;
const char *atffn = NULL;
List *curr_list = NULL;
List **siglists = NULL;
FILE *sigsfp = NULL;

void
save_L(unsigned char *s)
{
  s = (ucp)strchr((ccp)s, '\t');
  unsigned char *l = ++s;
  s = (ucp)strchr((ccp)s, '\t');
  *s++ = '\0';
  l = pool_copy(l, p);
  s = (ucp)strchr((ccp)s, '\t');
  unsigned char *n = pool_copy(s+1, p);
  fprintf(stderr, "save_L %s = %s\n", l, n);
  hash_add(L, l, n);
  int i = atoi((ccp)n);
  if (i > L_max)
    L_max = i;
}

void
save_l(unsigned char *s)
{
  s = (ucp)strchr((ccp)s, '\t');
  list_add(curr_list, pool_copy(++s, p));
}

List *
sigs_L(unsigned char *s)
{
  s = (ucp)strchr((ccp)s, '\t');
  const char *l = hash_find(L, ++s);
  assert(l);
  curr_line = atoi(l);
  if (!siglists[curr_line])
    siglists[curr_line] = list_create(LIST_SINGLE);
  fprintf(stderr, "sigs_L: addings sigs to line %d\n", curr_line);
  return siglists[curr_line];
}

#define form_char(fp,c,v) fprintf(fp,"\t%s%s",c,((!oconll&&v)?v:""))

void
form_serialize_tab(FILE *f_f2, Form *f)
{
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
sigs_out(void *vp)
{
  Form f2;
  memset(&f2,'\0',sizeof(Form));
  char *wid = vp;
  char *sig = strchr(wid, '\t');
  *sig++ = '\0';
  char *tab = strchr(sig, '\t');
  if (tab)
    *tab = '\0';
  if ('@' == *sig)
    {
      form_parse((uccp)"<tok>", 0, (ucp)sig, &f2, NULL);
      fprintf(sigsfp, "#sig:\t%s\t", wid); form_serialize_tab(sigsfp, &f2);
    }
  else
    {
      fprintf(sigsfp, "#sig:\t%s\t%s", wid, sig);
    }
  fputc('\n', sigsfp);
}

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
    
int
main(int argc, char *const *argv)
{
  L = hash_create(1024);
  p = pool_init();
  sigsfp = stdout;
  tokfn = argv[1];
  FILE *tokfp = xfopen(tokfn, "r");
  if (tokfp)
    {
      unsigned char *s;
      while ((s = loadoneline(tokfp, NULL)))
	{
	  if ('L' == *s)
	    save_L(s);
	  else if ('Y' == *s && Y_xcl(s))
	    break;
	}
      siglists = calloc(L_max+1, sizeof(void*));
      while ((s = loadoneline(tokfp, NULL)))
	{
	  if ('L' == *s)
	    curr_list = sigs_L(s);
	  else if ('l' == *s)
	    save_l(s);
	}
      xfclose(argv[1], tokfp);
      FILE *atffp = xfopen(argv[2], "r");
      if (atffp)
	{
	  int n = 0;
	  while ((s = loadoneline(atffp, NULL)))
	    {
	      fputs((ccp)s, sigsfp); fputc('\n', sigsfp);
	      if ('#' == s[0] && '#' == s[1]
		  && (!strncmp((ccp)s, "##file", 6) || !strncmp((ccp)s, "##line", 6)))
		fprintf(stderr, "skipping ++n for %s\n", s); /* don't increment line number for these */
	      else
		++n; /* we are consistently numbering from 1 including the siglists array */
	      List *lsigs = NULL;
	      if (n <= L_max && (lsigs = siglists[n]))
		{
		  fprintf(stderr, "siglists[n] has %d sigs\n", (int)list_len(siglists[n]));
		  do
		    {
		      s = loadoneline(atffp, NULL);
		      ++n;
		      fputs((ccp)s, sigsfp); fputc('\n', sigsfp);
		    }
		  while (s && strncmp((ccp)s, "#lem", 4));
		  /* Now we just dumped the #lem: line so dump the #sig: lines to go with it */
		  list_exec(lsigs, sigs_out);
		}
	    }
	}
    }
}
