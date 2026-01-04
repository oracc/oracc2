#include <oraccsys.h>

enum itype { 
  Photo,
  tPhoto,
  Line,
  tLine,
  Detail,
  tDetail,
  lDetail,
  tlDetail,
  SVG,
  tSVG,
  lSVG,
  ldSVG,
};

typedef struct img
{
  int type;
  const char *name;
} Img;

static int
cmp_imgp(const void *a, const void *b)
{
  Img *ai = *(Img**)a;
  Img *bi = *(Img**)b;
  return ai->type - bi->type;
}

int
main(int argc, char **argv)
{
  Hash *h = hash_create(8192);
  Memo *m = memo_init(sizeof(Img), 8192);
  const char *lp;
  size_t lplen;
  while ((lp = (ccp)loadoneline(stdin, &lplen)))
    {
      const char *P = strchr(lp, 'P');
      char Pbuf[8];
      strncpy(Pbuf, P, 7);
      Pbuf[7] = '\0';
      List *llp = hash_find(h, (uccp)Pbuf);
      if (!llp)
	{
	  llp = list_create(LIST_SINGLE);
	  hash_add(h, (uccp)strdup(Pbuf), llp);
	}
      Img *img = memo_new(m);
      img->name = strdup(lp);
      int svg = (strstr(lp, ".svg") ? 1 : 0);
      const char *tn = strstr(lp, "tn_");
      if (svg)
	{
	  if (strstr(lp, "_ld"))
	    img->type = ldSVG;
	  else if (strstr(lp, "_l."))
	    img->type = lSVG;
	  else
	    img->type = SVG;
	}
      else
	{
	  if (strstr(lp, "_ld"))
	    img->type = tn ? tlDetail : lDetail;
	  else if (strstr(lp, "_l"))
	    img->type = tn ? tLine : Line;
	  else if (strstr(lp, "_d"))
	    img->type = tn ? tDetail : Detail;
	  else
		 img->type = tn ? tPhoto : Photo;
	}
      list_add(llp, img);
    }

  int nkeys;
  const char **pkeys = hash_keys2(h, &nkeys);
  qsort(pkeys, nkeys, sizeof(const char *), cmpstringp);
  fprintf(stdout, "id_text\timages\n");
  size_t i;
  for (i = 0; i < nkeys; ++i)
    {
      List *lp = hash_find(h, (uccp)pkeys[i]);
      int nip;
      Img **ip = (Img**)list2array_c(lp, &nip);
      qsort(ip, nip, sizeof(Img *), cmp_imgp);
      fprintf(stdout, "%s\t", pkeys[i]);
      int j;
      for (j = 0; j < nip; ++j)
	{
	  if (j)
	    fputc(' ', stdout);
	  fputs(ip[j]->name, stdout);
	}
      fputc('\n', stdout);
    }
}
