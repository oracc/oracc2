#include <stdio.h>
#include <stdlib.h>
int
main(int argc, char **argv)
{
  int ch;
  while (EOF != (ch = getchar()))
    if (ch > 127)
      {
	puts("#atf: use unicode");
	exit(1);
      }
  exit(0);
}
