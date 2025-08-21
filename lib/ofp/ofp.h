#ifndef OFP_H_
#define OFP_H_

#include "osl_unicode.h"

typedef struct Ofp_header
{
  const char *label;
  const char *title;
  const char *page;
  const char *font;
  const char *css;
  const char *mag;
  const char *list;
  const char *data;
  const char *path;
} Ofp_header;

/* When indexing by U+ or u123AF names we use the 5-hex-digit portion
   only for all hash keys */
typedef struct Ofp
{
  const char *name;
  const char *file;
  int nglyphs;
  struct Ofp_glyph *glyphs;
  Hash *h_sign;
  Hash *h_glyf; /* glyphs that are not .liga */
  Hash *h_liga; /* glyphs that are .liga */
  int nsigns;
  struct Ofp_sign *signs;
  Memo *m_sign;
  Memo *m_liga;
  Pool *p;
  Osl_unicode *osl;
  Osl_unicode *pcsl;
  struct Ofp_list *list;
  Hash *h_list;
  Ofp_header h;
  FILE *trace;
} Ofp;

typedef enum Ofp_feature
  {
    OFPF_BASE , OFPF_LIGA , OFPF_SSET , OFPF_CVNN , OFPF_OIVS , OFPF_SALT , OFPF_NONE
  } Ofp_feature;
extern const char *ofp_feat_str[];

typedef struct Ofp_glyph
{
  int index;
  const char *key;
  const char *name;
  const char *code;  /* the derived value of the code from or u123AF name */
  const char *fcode; /* the font's value of the code */
  Ofp_feature f;
  int f_int;
  const char *f_chr;
  const char *ligl;
  const char *liga;
  const char *useq; /* liga as useq i.e., u12345_u12345 => x12345.x12345 */
  const char *ivs;
  List *ligas;	    /* liga entries which are not sign entries,
		       grouped under their ligl; note that sign entry
		       only applies if there is a list--if no list
		       then we list all liga under their ligl */
  int is_liga; 	    /* 1 if this entry is a ligature which is listed under another sign */
  Osl_uentry *osl;
  const char*gcomp; /* prebuilt char if this glyph is a .liga */
  const char*gliga; /* ligature if this glyph is a PUA used as a component */
} Ofp_glyph;

typedef struct Ofp_sign
{
  Ofp_glyph *glyph;
  List *salts;
  List *cvnns;
  List *ssets;
  List *oivs;
  List *ligas;
} Ofp_sign;

typedef struct Ofp_liga
{
  Ofp_glyph *glyph;
  List *salts;
  List *cvnns;
  List *ssets;
  List *oivs;
} Ofp_liga;

typedef struct Ofp_list
{
  const char *l; /* list */
  const char *o; /* oid */
  const char *f; /* OT features */
  int s; /* sort code */
  struct Ofp_list *next;
} Ofp_list;

typedef struct ofp_bv_arg { Ofp *o; const char *code; FILE *fp; } ofp_bv_arg;

enum ofpdefs_val { ofpdefs_label, ofpdefs_title, ofpdefs_page,
		   ofpdefs_font, ofpdefs_css, ofpdefs_mag, ofpdefs_list,
		   ofpdefs_data, ofpdefs_path };
struct ofpdefs_tab { const char *name; enum ofpdefs_val v; };

extern Ofp *ofp_init(void);
extern void ofp_term(Ofp *ofp);
extern void ofp_list(Ofp *ofp, const char *larg);
extern Ofp *ofp_load(const char *font, int tracing);
extern void ofp_dump(Ofp *ofp, FILE *fp);
extern void ofp_ingest(Ofp *ofp);
extern void ofp_marshall(Ofp *ofp);
extern void ofp_xml(Ofp *ofp, FILE *fp);
extern void ofp_debug(Ofp *ofp, const char *fname);
extern char *liga2useq(Ofp *ofp, const char *liga);
extern Osl_uentry *osl_autocreate(const char *u);
extern struct ofpdefs_tab *ofpdefs (register const char *str, register size_t len);
extern void ofp_header(Ofp *o, const char *def);
#endif/*OFP_H_*/
