#include <oraccsys.h>

int complement = 0;
int percent = 0;
int verbose = 0;
int
main(int argc, char **argv)
{
  const char *pct = "";
  options(argc,argv,"cpv");
  double amt = strtod(argv[optind++], NULL);
  double tot = strtod(argv[optind++], NULL);
  if (verbose)
  printf("args: amt = %.g; tot=%.g\n", (float)amt, (float)tot);
  if (complement)
    amt = tot - amt;
  if (percent)
    pct = "%";
  if (verbose)
    printf("%.g/%.g as %% = %.2g%s\n", (float)amt, (float)tot, (float)pctd(amt,tot), pct);
  else
    {
      char buf[7];
      sprintf(buf,"%.2f", pctd(amt,tot));
      int i = strlen(buf);
      while ('0' == buf[i-1])
	{
	  buf[i-1] = '\0';
	  --i;
	}
      if ('.' == buf[i-1])
	buf[i-1] = '\0';
      printf("%s%s\n", buf, pct);
    }
}

int opts(int argc, const char *arg)
{  
  switch (argc)
    {
    case 'c':
      complement = 1;
      break;
    case 'p':
      percent = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}
void help (void) { }
