#ifndef TSV_H_
#define TSV_H_

#include <dbi.h>

typedef struct tsv_data
{
  long int seek;
  int len;
  int count;
} Tsv_data;

typedef struct tsv
{
  const char *tsv_fn;
  FILE *tsv_fp;
  FILE *out_fp;
  const char *tdb_dir;
  const char *tdb_name;
  Dbi_index *dp;
  int count_items;
  int free_dir;
  int free_name;
  Tsv_data data;
} Tsv;

extern int space_newlines;

#include "all.proto"

#endif/*TSV_H_*/
