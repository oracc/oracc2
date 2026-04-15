#include <oraccsys.h>

int
main(int argc, char *const *argv)
{
  msort_options(argc, argv);
  unsigned char **lines;
  unsigned char *mem;
  size_t nlines;
  lines = loadfile_lines3((uccp)argv[1], &nlines, &mem);
  if (nlines)
    qsort(lines, nlines, sizeof(char *), cmpu8normp);
  int i;
  for (i = 0; i < nlines; ++i)
    printf("%s\n", (ccp)lines[i]);
}
