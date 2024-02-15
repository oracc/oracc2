#include <oraccsys.h>
#include <roco.h>

int
main(int argc, char **argv)
{
  Hash *seen = hash_create(1024);
  Roco *r = roco_load1("-");
  int i;
  printf("const x = {\n");
  for (i = 0; i < r->nlines; ++i)
    {
      if (!hash_find(seen, r->rows[i][0]))
	{
	  hash_add(seen, r->rows[i][0], "");
	  if (i)
	    fputs(",\n",stdout);
	  printf("\"%s\":\"%s\"", r->rows[i][0], r->rows[i][1]);
	}
    }
  printf("\n};");
}
