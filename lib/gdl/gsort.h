#ifndef GSORT_H_
#define GSORT_H_

struct GS_mods
{
  char type;
  char *val;
  struct GS_mods *next;
};

enum GS_type { GST_REG , GST_ZATU , GST_PUNCT , GST_NUM , GST_N57 };

struct GS_item
{
  unsigned const char *g;	/* item grapheme */
  int t;			/* grapheme type: see GS_type */
  unsigned const char *b;	/* grapheme base */
  unsigned const char *k;	/* grapheme key via lib/collate; this is stripped of mods */
  unsigned const char *m; 	/* mods for grapheme, "" if none */
  struct GS_mods *mp;
  unsigned const char *s;	/* suffix for lists */
  short x;			/* grapheme index */
  double r;			/* repeater for number graphemes, e.g., 3(diš); may be fraction, e.g., 1/2(diš) r=0.5 */
};

struct GS_head
{
  unsigned const char *s; 	/* original string */
  int n;		  	/* number of items */
  struct GS_item **i;		/* array of item ptrs */
};

typedef struct GS_head GS_head;
typedef struct GS_item GS_item;
typedef struct GS_mods GS_mods;

extern void gsort_init(void);
extern void gsort_term(void);
extern GS_head *gsort_prep(Tree *tp);
extern void gsort_show(GS_head *gsp);
extern void gsort_show_sub(FILE *fp, GS_head *gsp);
extern int gsort_cmp(const void *v1, const void *v2);
extern int gsort_ud_md_test(unsigned const char *t);
extern GS_head *gsort_ud_md(Tree *tp);

extern int gsort_trace;

#endif/*GSORT_H_*/
