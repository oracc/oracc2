#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "oracclocale.h"
#include "hash.h"
#include "runexpat.h"
#include "xmlns_lib.h"

int verbose;

int
main(int argc, char **argv)
{
  setlocale(LC_ALL,ORACC_LOCALE);
  const char *ns = xmlns(argv[1]);
  printf("%s", ns);
}

const char *prog = "xmlns";
int major_version = 1, minor_version = 0;
const char *usage_string = "xmlns XMLFILE";
void help (void) { }
int opts(int arg,char*str){ return 1; }
