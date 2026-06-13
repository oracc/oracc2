#include <oraccsys.h>
#undef VECTEST
int
main(int argc, char *const *argv)
{
  size_t siz;
  unsigned char **p = loadfile_lines((uccp)argv[1], &siz);
  /* sort -u cat-ids.lst guarantees last is highest P-num */
  int last = strtoul((ccp)p[siz-1]+1,NULL,10);
  int vecsiz = (last / 8) + 1;
  unsigned char *v = calloc(vecsiz,sizeof(unsigned char));
  int i;
  for (i = 0; p[i]; ++i)
    {
      unsigned long l = strtoul((ccp)p[i]+1, NULL, 10);
      bit_set(v[l/8],1<<(l%8));
    }
#ifdef VECTEST
  for (i = 0; i <= last; ++i)
    if (bit_get(v[i/8],1<<(i%8)))
      printf("P%06d\n",i);
#else
  fwrite(v, sizeof(unsigned char), vecsiz, stdout);
#endif
}
