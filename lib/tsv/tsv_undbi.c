#include <oraccsys.h>
#include "tsv.h"

#define tsv_undbi_str(f) while(EOF!=(ch=fgetc(f))){fputc(o,ch);if (!ch)break;}
#define tsv_undbi_skip4(f) for(i=0;i<4;++i){(void)fgetc(f);}

int
tsv_undbi(Tsv *tp)
{
  char dbi[strlen(tp->tdb_dir)+strlen(tp->tdb_name)+strlen("/.dbi0")];
  sprintf(dbi, "%s/%s.dbi", tp->tdb_dir, tp->tdb_name);
  FILE *fp = fopen(dbi, "r");
  if (!fp)
    return 1;
  
  Unsigned32 siz;
  fscanf(fp, "%d", &siz);
  if (siz != 1)
    return 1;

  while (!feof(fp))
    {
      tsv_undbi_str(fp);
      tsv_undbi_skip4(fp);
      fputc(o,'\t');
      tsv_undbi_str(fp);
      fputc(o,'\n');
    }
}
