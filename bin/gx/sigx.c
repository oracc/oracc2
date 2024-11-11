#include <oraccsys.h>
#include <form.h>
#include <lng.h>

int
main(int argc, char **argv)
{
  unsigned char *lp;
  Form f;
  int ln = 0;
  ccp lang;
  mesg_init();
  form_init();
  while ((lp = loadoneline(stdin, NULL)))
    {
      if (form_parse((uccp)"<stdin>", ++ln, lp, &f, NULL) >= 0)
	{
	  if (f.pos && isupper(f.pos[0]) && 'N' == f.pos[1])
	    lang = "qpn";
	  else
	    lang = f.core->name;
	  printf("%s\t%s[%s]%s\tword\n", lang, f.cf, f.gw, f.pos);
	  printf("%s\t%s[%s//%s]%s'%s\tsense\t%s[%s]%s\tword\n",
		 lang, f.cf, f.gw, f.sense, f.pos, f.epos, f.cf, f.gw, f.pos);
	}
      else
	{
	  mesg_print(stderr);
	  mesg_init();
	}
    }
}
