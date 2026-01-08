#ifndef KEYDATA_H_
#define KEYDATA_H_

typedef struct key
{
  const char *class;
  const char *method;
  int queryable;
  int remap;
  const char *remapto;
  int reorder;
  const char *type;
  List *lvals; /* preserve order of key values for sorting */
  Hash *hvals;
} Key;

typedef struct val
{
  const char *v; /* value in incoming data */
  const char *r; /* value to remap v to */
} Val;

typedef struct keydata
{
  const char *file;
  Hash *notindexed;	/* key is field name; val is "" */
  Hash *keytypes;	/* key is field name; val = field type */
  Hash *sortable;	/* key is field type; val = human-readable version of field type */
  Hash *hkeys;		/* key is 'type' attr; val = Key ptr */
  Memo *mkey;
  Memo *mval;
  Pool *p;	/* regular pool */
} Keydata;


const char *keydata_find(const char *project);
Keydata *keydata_init(const char *file);
void keydata_load(Keydata *kp);
void keydata_term(Keydata *kp);

#endif/*KEYDATA_H_*/
