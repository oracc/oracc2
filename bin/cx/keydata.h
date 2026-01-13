#ifndef KEYDATA_H_
#define KEYDATA_H_

#include "cx.h"

typedef struct kdkey
{
  int remap;
  int closed;
  int reorder;
  int queryable;
  const char *method;
  const char *remapto;
  const char *type;
  List *lvals; /* preserve order of key values for sorting */
  Hash *rvals; /* hash of value to remapped value */
  Hash *hvals; /* hash of known values */
} KD_key;

typedef struct kdval
{
  const char *v; /* value in incoming data */
  const char *r; /* value to remap v to */
} KD_val;

typedef struct kdmap
{
  List *fields;
  const char *hr;
} KD_map;

typedef struct keydata
{
  Cx *c;
  const char *file;
  Hash *notindexed;	/* key is field name; val is "" */
  Hash *keytypes;	/* key is field name; val = field type */
  Hash *needtype;	/* key is type referenced in keytype@type; val = "" */
  Hash *sortable;	/* key is field type; val = human-readable version of field type */
  Hash *hkeys;		/* key is 'type' attr; val = Key ptr */
  Memo *mkey;
  Memo *mval;
  Memo *mmap;
  char **fields;
  int nfields;
  int nmapentries;
  char **fields_used;
  int nfields_used;
  int nmapentries_used;
  Hash *hfields;	/* fields that are wanted from keydata */
  Pool *p;
} Keydata;


extern char *keydata_find(const char *project);
extern Keydata *keydata_init(Cx *c, const char *file, Hash *hfields);
extern void keydata_load(Keydata *kp);
extern void keydata_term(Keydata *kp);
extern int keydata_validate(Keydata *kp);

#endif/*KEYDATA_H_*/
