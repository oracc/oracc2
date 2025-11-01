%{
#include <string.h>
#include "bx.h"
#include "bib.h"
%}
struct bib_fld_tab;
%%
address,	f_address
author,	f_author
authortype,	f_authortype
bookauthor,	f_bookauthor
booksubtitle,	f_booksubtitle
booktitle,	f_booktitle
chapter,	f_chapter
date,	f_date
doi,	f_doi
edition,	f_edition
editor,	f_editor
entrysubtype,	f_entrysubtype
eprint,	f_eprint
eprinttype,	f_eprinttype
event,	f_event
eventdate,	f_eventdate
eventtitle,	f_eventtitle
eventvenue,	f_eventvenue
ids,	f_ids
institution,	f_institution
isbn,	f_isbn
issn,	f_issn
journal,	f_journal
keywords,	f_keywords
langid,	f_langid
location,	f_location
month,	f_month
note,	f_note
number,	f_number
page,	f_page
pages,	f_pages
part,	f_part
publisher,	f_publisher
related,	f_related
relatedtype,	f_relatedtype
school,	f_school
series,	f_series
shorthand,	f_shorthand
subtitle,	f_subtitle
title,	f_title
translator,	f_translator
type,	f_type
url,	f_url
venue,	f_venue
volume,	f_volume
year,	f_year
%%

static void
bib_fld_set_val(const char *name, bibvalfnc v)
{
    struct bib_fld_tab *fp = bib_fld(name, strlen(name)); fp->v = v;
}
void
bib_fld_tab_init(void)
{
  bib_fld_set_val("author", bvl_name);
  bib_fld_set_val("bookauthor", bvl_name);
  bib_fld_set_val("editor", bvl_name);
  bib_fld_set_val("translator", bvl_name);
  bib_fld_set_val("ids", bvl_ids);
  bib_fld_set_val("page", bvl_page);
  bib_fld_set_val("pages", bvl_page);
  bib_fld_set_val("year", bvl_year);
}
