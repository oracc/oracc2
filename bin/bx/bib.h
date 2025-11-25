#ifndef BIB_H_
#define BIB_H_

#include <memo.h>
#include <mesg.h>
#include <runexpat.h>

enum bib_etype {
  e_article, e_book, e_booklet, e_collection, e_dataset,
  e_inbook, e_incollection, e_inproceedings, e_mastersthesis,
  e_online, e_phdthesis, e_proceedings, e_report, e_software,
  e_techreport, e_unpublished, e_top };

enum bib_ftype {
  f_address, f_author, f_authortype, f_bookauthor, f_booksubtitle,
  f_booktitle, f_chapter, f_date, f_doi, f_edition, f_editor,
  f_entrysubtype, f_eprint, f_eprinttype, f_event, f_eventdate,
  f_eventtitle, f_eventvenue, f_ids, f_institution, f_isbn, f_issn,
  f_journal, f_keywords, f_langid, f_location, f_month, f_note,
  f_number, f_page, f_pages, f_part, f_publisher, f_related,
  f_relatedtype, f_school, f_series, f_shorthand, f_subtitle, f_title,
  f_translator, f_type, f_url, f_venue, f_volume, f_year, f_top
};

typedef struct bib
{
  const char *file;
  List *entries;
  Bx *bp;
} Bib;

struct bib_fld_tab;
struct name;

typedef struct bibentry
{
  const char *type;
  const char *bkey;	/* .bib key, the one after @article{ or the like */
  const char **aka;    	/* aka from @ids */
  const char *id;
  struct bibfield *fields[f_top];
  struct bibicf *icf;
  struct name **names;	/* author names */
  int nnames;
  struct name **enames; /* editor names */
  int nenames;
  const char *allnames; /* either author names or if no authors then editor names */
  int year;
  int sameauth; 	/* in sorted list, this entry has same author(s) as previous */
  int disamb;   	/* nth in sequence of disambiguated entries in sorted list */
  Bib *bib;
} Bibentry;

typedef struct bibfield
{
  const char *name;
  const char *data;
  int line;
} Bibfield;

typedef struct name
{
  const char *nkey;	/* name key, i.e., last + init with no '.' or spaces */
  const char *orig;
  const char *last;
  const char *rest;
  const char *init;
  const char *was;
  int sames;		/* this person's last name is same as another person's */
  int bm_name_xml;
} Name;

typedef struct bibicf
{
  const char *str;
  Bibentry *ep;
} Bibicf;

struct bib_year_tab { const char *name; int year; };
extern struct bib_year_tab *bib_year(register const char *str, register size_t len);

typedef void (*bibvalfnc)(Mloc *mp, Bx *bp, enum bib_ftype t, Bibentry *ep);
extern bibvalfnc bib_validators[f_top];

struct bib_ent_tab { const char *name; enum bib_etype t; };
struct bib_fld_tab { const char *name; enum bib_ftype t; int sort; };

extern struct bib_ent_tab *bib_ent(register const char *str, register size_t len);
extern struct bib_fld_tab *bib_fld(register const char *str, register size_t len);

extern void bnm_s_namemap(void *userData, const char *name, const char **atts);
extern void bnm_s_name(void *userData, const char *name, const char **atts);
extern void bnm_s_last(void *userData, const char *name, const char **atts);
extern void bnm_s_rest(void *userData, const char *name, const char **atts);
extern void bnm_s_was(void *userData, const char *name, const char **atts);
extern void bnm_s_aka(void *userData, const char *name, const char **atts);
extern void bnm_s_init(void *userData, const char *name, const char **atts);

extern void bnm_e_namemap(void *userData, const char *name);
extern void bnm_e_name(void *userData, const char *name);
extern void bnm_e_last(void *userData, const char *name);
extern void bnm_e_rest(void *userData, const char *name);
extern void bnm_e_was(void *userData, const char *name);
extern void bnm_e_aka(void *userData, const char *name);
extern void bnm_e_init(void *userData, const char *name);

struct bnm_tab { const char *name; XML_StartElementHandler s; XML_EndElementHandler e; };
extern struct bnm_tab *bnm_tab(register const char *str, register size_t len);

extern int biblineno, bib_key_next;
extern int bib_field, bib_nesting;
extern const char *curr_bib, *curr_key, *field, *fldnames[];
extern Bib *curr_bibp;
extern Bibentry *curr_ep;

extern void bib_entry_init(Mloc m, const char *ent, const char *key);
extern void bib_entry_term(Mloc m);
extern void bib_field_init(Mloc m, const char *f);
extern void bib_field_term(Mloc m);
extern char *bib_content(Mloc m, const char *bit);

extern const char * l_biblloc_file(void);
extern const int l_biblloc_line(void);

extern void bibreset(void);

extern void bib_xml(Bx *bp, List *b, FILE *fp);
extern void bib_bib(Bx *bp, List *b, FILE *fp);

extern Hash *bib_cites;
extern Mloc biblloc;
extern int bibparse(void);
extern void bib_init(Bx *bp);
extern void bibset_debug(int);
extern void bib_wrapup_buffer(void);

extern void bib_fld_tab_init(void);

extern void bib_sort(Bx *bp);
extern void bib_disambiguate(Bx *bp);

extern void bnm_init(Bx *bp);
extern void bnm_tab_init(void);
extern void bnm_all_names(Bibentry *ep, enum bib_ftype t);
extern void bnm_nkey(Mloc *mp, Bx *bp, Name *np);
extern void bnm_people(Bx *bp);
extern void bnm_split(Mloc *mp, Bx *bp, Bibentry *ep, Name *np);

extern void bvl_init(void);
extern void bvl_ids(Mloc *mp, Bx *bp, enum bib_ftype t, Bibentry *ep);
extern void bvl_page(Mloc *mp, Bx *bp, enum bib_ftype t, Bibentry *ep);
extern void bvl_name(Mloc *mp, Bx *bp, enum bib_ftype t, Bibentry *ep);
extern void bvl_year(Mloc *mp, Bx *bp, enum bib_ftype t, Bibentry *ep);

#endif/*BIB_H_*/
