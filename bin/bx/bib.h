#ifndef BIB_H_
#define BIB_H_

#include <memo.h>
#include <mesg.h>

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
} Bib;

typedef struct bibentry
{
  const char *type;
  const char *key;
  const char *fields[f_top];
} Bibentry;

struct bib_ent_tab { const char *name; enum bib_etype t; };
struct bib_fld_tab { const char *name; enum bib_ftype t; };

extern struct bib_ent_tab *bib_ent(register const char *str, register size_t len);
extern struct bib_fld_tab *bib_fld(register const char *str, register size_t len);

extern int biblineno, bib_key_next;
extern int bib_field, bib_nesting;
extern const char *curr_bib, *curr_key, *field;
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

extern void bib_xml(List *b, FILE *fp);

extern Hash *bib_cites;
extern Mloc biblloc;
extern int bibparse(void);
extern void bib_init(Bx *bp);
extern void bibset_debug(int);
extern void bib_wrapup_buffer(void);

#endif/*BIB_H_*/
