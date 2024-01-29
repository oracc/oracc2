#include <oraccsys.h>
#include <vido.h>

int
main(int argc, char **argv)
{
  char buf[1024], *b, wdid[512];
  Vido *vp = vido_init('t', 0);
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
	  s = strchr(t, '\t');  /* signature */
	  if (s)
	    *s++ = '\0';
	  w = strchr(s, '\t');
	  if (w)
	    *w++ = '\0';
	  printf("%s\t%s\n", vido_new_id(vp,t), wdid);
	  /*printf("%s\t%s\n", s, wdid);*/
	  /* expand signature and generate sub--tokens */
	}
    }
  vido_dump_data(vp, "tid.vid");
  vido_term(vp);
}
