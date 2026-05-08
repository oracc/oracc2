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
  const char *base;
  const char *tsv_fn;
  FILE *tsv_fp;
  FILE *out_fp;
  const char *tdb_dir;
  const char *tdb_name;
  Dbi_index *dp;
  int count_items;
  int free_dir;
  int free_name;
  Tsv_data *data;
} Tsv;

extern int full_record, space_newlines;

/* tdb_access.c */
extern void tdb_open(Tsv *tp);
extern int tdb_exists(Tsv *tp, const char *id);
extern int tdb_find(Tsv *tp, const char *id);
extern void tdb_close(Tsv *tp);
extern int tdb_one_off(Tsv *tp, const char *id);
extern int tdb_keys(Tsv *tp, const char **ids);
/* tdb_add.c */
extern int tdb_add(Tsv *tp, const char *id, long int s, int l, int n);
/* tdb_count_items.c */
extern void tdb_count_items(Tsv *tp);
/* tdb_lib.c */
extern void tdb_count_items(Tsv *tp);
/* tsv_create.c */
extern Tsv *tsv_create(void);
extern void tsv_destroy(Tsv *tp);
/* tsv_index.c */
extern int tsv_index(const char *tsv_file, const char *dir, const char *name);
/* tsv_init.c */
extern Tsv *tsv_init(const char *tsv_file, const char *dir, const char *name);
extern void tsv_term(Tsv *tp);
/* tsv_one_off.c */
extern int tsv_one_off(const char *tsv_fn, const char *dir, const char *name, const char *key);
/* tsv_output.c */
extern int tsv_output(Tsv *tp);
/* tsv_paths.c */
extern void tsv_paths(Tsv *tp, const char *tsvfn, const char *name, const char *dir);
/* tsv_undbi.c */
extern int tsv_undbi(const char *tsv_file, const char *dir, const char *name);

#endif/*TSV_H_*/
