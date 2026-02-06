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

#define kis_tis(r) (kis->(r)[0])
#define kis_key(r) (kis->(r)[1])
#define kis_cnt(r) (kis->(r)[2])
#define kis_pct(r) (kis->(r)[3])

extern Kis *kis_load(const char *fn);

#endif/*KIS_H_*/
