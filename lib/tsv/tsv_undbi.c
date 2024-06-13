#include <oraccsys.h>
#include "tsv.h"

#define tsv_undbi_str(f) while(EOF!=(ch=fgetc(f))){fputc(ch,tp->out_fp);if (!ch)break;}
#define tsv_undbi_skip4(f) for(i=0;i<4;++i){(void)fgetc(f);}

int
tsv_undbi(const char *tsv_file, const char *dir, const char *name)
{
  Tsv *tp = tsv_create();
  tsv_paths(tp, tsv_file, name, dir);
  tp->out_fp = stdout;
  char dbi[strlen(tp->tdb_dir)+strlen(tp->tdb_name)+strlen("/.dbi0")];
  sprintf(dbi, "%s/%s.dbi", tp->tdb_dir, tp->tdb_name);
  FILE *fp = fopen(dbi, "r");
  if (!fp)
    return 1;
  
  Unsigned32 siz;
  fread(&siz, sizeof(Unsigned32), 1, fp);
  if (siz != 1)
    return 1;

  while (!feof(fp))
    {
      int ch, i;
      tsv_undbi_str(fp);
      if (!feof(fp))
	{
	  tsv_undbi_skip4(fp);
	  fputc('\t',tp->out_fp);
	  tsv_undbi_str(fp);
	  fputc('\n',tp->out_fp);
	}
    }

  fclose(fp);
  return 0;
}
