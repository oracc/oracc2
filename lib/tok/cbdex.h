#ifndef CBDEX_H_
#define CBDEX_H_

struct cbdex_header
{
  Memo *m;
  Pool *p;
};

struct cbdex
{
  const char *tok;
  const char *lem;
  const char *oid;  /* OID of lemma */
  const char *lcnt; /* count of lemma occurrences */
  const char *base; /* base as from the @n attribute */
  const char *bcnt; /* count of base occurrences */
  const char *gpos; /* grapheme position (from no_determinative position) */
};

extern struct cbdex *cbdex_init(void);
extern void cbdex_term(struct cbdex *cp);
extern void cbdex_loc(char *lp, struct cbdex *cp);
extern void cbdex_g(char *g, struct cbdex *cp);
extern void cbdex_parse(char *c, struct cbdex *cp);
extern void cbdex_write(FILE *fp, struct cbdex *cp);

#endif/*CBDEX_H_*/
