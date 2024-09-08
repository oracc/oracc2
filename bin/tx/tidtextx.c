#include <stdio.h>
#include <stdlib.h>

/** prepend id_text to CDLI catalog output which has artifact_id in
 *  col1
 */

int
main(int argc, char **argv)
{
  int ch;
  fputs("id_text\t", stdout);
  while (EOF != (ch=getchar()))
    {
      putchar(ch);
      if ('\n' == ch)
	break;
    }
  int lnum = 2;
  while (!feof(stdin))
    {
      int iid;
      int ret = scanf("%i",&iid);
      if (ret)
	{
	  printf("P%06d\t%d", iid, iid);
	  while (EOF != (ch=getchar()))
	    {
	      putchar(ch);
	      if ('\n' == ch)
		{
		  ++lnum;
		  break;
		}
	    }
	}
      else
	{
	  fprintf(stderr, "tidtextx: bad artifact_id at line %d\n", lnum);
	}
    }
}
