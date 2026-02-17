#include <oraccsys.h>
#include <uninorm.h>

int
cmpstringp(const void *p1, const void *p2)
{
  return strcmp(* (char * const *) p1, * (char * const *) p2);
}

int
cmpu8normp(const void *p1, const void *p2)
{
  int c;
  int res = u8_normcmp(*(unsigned char*const*)p1, strlen(*(char*const*)p1),
		       *(unsigned char*const*)p2, strlen(*(char*const*)p2),
		       UNINORM_NFD, &c);
  if (res == -1)
    {
      fprintf(stderr, "cmpunormp: u8_normcmp returned error\n");
      return 0;
    }
  else
    return c;
}
