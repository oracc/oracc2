#ifndef KIS_H_
#define KIS_H_

typedef struct kis
{
  char *isid; /* instance set ID */
  char *key;  /* sign list or glossary key */
  char *cnt;  /* count of occurrences */
  char *pct;  /* percentage (relative to different fields) */
} Kis;

#endif/*KIS_H_*/
