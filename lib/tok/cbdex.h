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
  struct cbdex_header *chp;
};

#endif/*CBDEX_H_*/
