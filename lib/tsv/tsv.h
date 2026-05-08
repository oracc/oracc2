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
  const char **keys;
  int nkey;
  Tsv_data *data;
} Tsv;

extern int full_record, space_newlines;

extern void tdb_open(Tsv *tp);
extern int tdb_exists(Tsv *tp, const char *id);
extern int tdb_find(Tsv *tp, const char *id, int i);
extern void tdb_close(Tsv *tp);
extern int tdb_lookup(Tsv *tp);
extern int tdb_add(Tsv *tp, const char *id, long int s, int l, int n);
extern void tdb_count_items(Tsv *tp);
extern void tdb_count_items(Tsv *tp);
extern Tsv *tsv_create(void);
extern void tsv_destroy(Tsv *tp);
extern int tsv_index(const char *tsv_file, const char *dir, const char *name);
extern Tsv *tsv_init(const char *tsv_file, const char *dir, const char *name);
extern int tsv_lookup(const char *tsv_fn, const char *dir, const char *name, const char **keys, size_t nkeys);
extern void tsv_term(Tsv *tp);
extern int tsv_one_off(const char *tsv_fn, const char *dir, const char *name, const char *key);
extern int tsv_output(Tsv *tp);
extern void tsv_paths(Tsv *tp, const char *tsvfn, const char *name, const char *dir);
extern int tsv_undbi(const char *tsv_file, const char *dir, const char *name);

#endif/*TSV_H_*/
