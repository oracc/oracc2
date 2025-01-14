#include <oraccsys.h>

#define iPhoto  0
#define tPhoto  1
#define iLine   2
#define tLine   3
#define iDetail 4
#define tDetail 5
#define iSVG    6

typedef struct img
{
  int type;
  const char *name;
} Img;

static int
cmp_imgp(const void *a, const void *b)
{
  Img *ai = a;
  Img *bi = b;
  return a->type - b->type;
}

int
main(int argc, char **argv)
{
  Hash *h = hash_create(8192);
  Memo *m = mem_init(sizeof(Img), 8192);
  const char *lp;
  size_t lplen;
  while ((lp = loadoneline(stdin, &lplen)))
    {
      const char *P = strchr(lp, 'P');
      char Pbuf[8];
      strncpy(Pbuf, P, 7);
      Pbuf[7] = '\0';
      List *lp = hash_find(h, Pbuf);
      if (!lp)
	{
	  lp = list_create(LIST_SINGLE);
	  hash_add(h, strdup(Pbuf), lp);
	}
      img = memo_new(m);
      img->name = strdup(lp);
      const char *tn = strstr(lp, "tn_");
      if (strstr(lp, "_l"))
	img->type = tn ? tLine : iLine;
      else if (strstr(lp, "_d"))
	img->type = tn ? tDetail : iDetail;
      else if (strstr(lp, ".jpg"))
	img->type = tn ? tPhoto : iPhoto;
      else
	img->type = iSVG;
      list_add(lp, img);
    }

  const char **pkeys = hash_keys2(h, &lplen);
  qsort(pkeys, lplen, sizeof(const char *), cmp_stringp);
  size_t i;
  for (i = 0; i < lplen; ++i)
    {
      List *lp = hash_find(h, pkeys[i]);
      int *nip;
      Img *ip = list2array(lp, &nip);
      qsort(ip, nip, sizeof(Img *), cmp_imgp);
      fprintf(stdout, "%s\t", pkeys[i]);
      int j;
      for (j = 0; j < nip; ++j)
	{
	  fputs(ip[j]->name, stdout);
	  if (j)
	    fputc(' ', stdout);
	}
      fputc('\n', stdout);
    }
}
