#include <oraccsys.h>
#include <atf2utf.h>

static void
xutf8ify(unsigned char *b, unsigned int c)
{
  if (c<0x80) *b++=c;
  else if (c<0x800) *b++=192+c/64, *b++=128+c%64;
  else if (c-0xd800u<0x800) goto error;
  else if (c<0x10000) *b++=224+c/4096, *b++=128+c/64%64, *b++=128+c%64;
  else if (c<0x110000) *b++=240+c/262144, *b++=128+c/4096%64, *b++=128+c/64%64, *b++=128+c%64;
  *b = '\0';
  return;
 error:
  b[0] = '\0';
}

int
main(int argc, char **argv)
{
  unsigned char *lp;
  unsigned char render[1024];
  while ((lp = loadoneline(stdin, NULL)))
    {
      if ('#' == *lp)
	{
	  unsigned char *ren = lp+1;
	  while (isspace(*ren))
	    ++ren;
	  if (!strncmp((char*)ren, "ren", 3))
	    {
	      ren += 3;
	      while (isspace(*ren))
		++ren;
	      strcpy((char*)render, (char*)ren);
	    }
	}
      else
	{
	  unsigned int chr, ivs;
	  sscanf((char*)lp, "%x %x", &chr, &ivs);
	  unsigned char uchr[10];
	  xutf8ify(uchr, chr);
	  unsigned char uivs[10];
	  xutf8ify(uivs, ivs);
	  unsigned char *ident = (ucp)strrchr((ccp)lp, ';');
	  ++ident;
	  while (isspace(*ident))
	    ++ident;
	  unsigned char *e = lp+strlen((ccp)lp);
	  while (isspace(e[-1]))
	    --e;
	  if (*e)
	    *e = '\0';
	  /*printf("chr=%d (%x) = %s; ivs=%x = %s\n", chr, chr, uchr, ivs, uivs);*/
	  if ('x' == render[0])
	    {
	      unsigned int rench;
	      sscanf((char*)render+1, "%x", &rench);
	      xutf8ify(render, rench);
	    }
	  printf("%s\t%s%s\t%s\t%s\n", uchr, uchr, uivs, render, ident);
	}
    }
}
