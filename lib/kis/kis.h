#ifndef KIS_H_
#define KIS_H_

#if 0
/* Not clear that the struct interface will be needed */
typedef struct kis
{
  char *tis; /* token instance set ID */
  char *key;  /* sign list or glossary key */
  char *cnt;  /* count of occurrences */
  char *pct;  /* percentage (relative to different fields) */
} Kis;
#endif

/* Macros to access Roco rows as strings */

typedef struct roco Kis;
typedef const char ** Kis_data;

#define kis_tis(r) ((r)[0])
#define kis_key(r) ((r)[1])
#define kis_cnt(r) ((r)[2])
#define kis_pct(r) ((r)[3])

extern Kis *kis_load(const char *fn);

extern cmp_fnc_t kis_cmp(enum efield f);

#if 0
extern int kis_unicode_cmp(const void *a, const void *b);
extern int kis_grapheme_cmp(const void *a, const void *b);
#endif

#endif/*KIS_H_*/
