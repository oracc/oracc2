#include <oraccsys.h>

int
main(int argc, char *const *argv)
{
  size_t nb;
  char *b;
  while ((b = (char*)loadoneline(stdin, &nb)))
    {
      char *lig = strstr(b, "<-");
      if (lig)
	{
	  lig += 3;
	  while ('u' == *lig)
	    {
	      ++lig;
	      fputs((ccp)utf8ify((unsigned int)strtol(lig,NULL,16)), stdout);
	      lig = strchr(lig, '_');
	      if (lig)
		++lig;
	      else
		break;
	    }
	  fputc('\n', stdout);
	}
    }
}
