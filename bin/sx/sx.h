#ifndef SX_H_
#define SX_H_
#include <roco.h>
#include <asl.h>

enum sx_pos_e { sx_pos_init , sx_pos_inst , sx_pos_term };

struct sx_functions;

typedef void (sx_signlist_f)(struct sx_functions *,struct sl_signlist*,enum sx_pos_e);
typedef void (sx_letter_f)(struct sx_functions*,struct sl_signlist*,struct sl_letter*,enum sx_pos_e);
typedef void (sx_group_f)(struct sx_functions*,struct sl_signlist*,struct sl_group*,enum sx_pos_e);
typedef void (sx_sign_f)(struct sx_functions*,struct sl_signlist*,struct sl_inst*,enum sx_pos_e);
typedef void (sx_list_f)(struct sx_functions*,struct sl_signlist*,struct sl_inst*,enum sx_pos_e);
typedef void (sx_form_f)(struct sx_functions*,struct sl_signlist*,struct sl_inst*,enum sx_pos_e);
typedef void (sx_value_f)(struct sx_functions*,struct sl_signlist*,struct sl_inst*,enum sx_pos_e);
typedef void (sx_notes_f)(struct sx_functions*,struct sl_signlist*,struct sl_inst*);
typedef void (sx_unicode_f)(struct sx_functions*,struct sl_signlist*,struct sl_unicode*);
typedef void (sx_String_f)(struct sx_functions*,struct sl_signlist*,unsigned const char *,enum sx_pos_e);
typedef void (sx_List_f)(struct sx_functions*,struct sl_signlist*,List *,enum sx_pos_e);

#include <stdio.h>

struct sx_functions
{
  sx_signlist_f *	sll;
  sx_letter_f *	let;
  sx_group_f *	grp;
  sx_sign_f *		sgn;
  sx_form_f *		frm;
  sx_list_f *		lst;
  sx_value_f *	val;
  sx_value_f *	inh;
  sx_value_f *	qvs;
  sx_notes_f *	not;
  sx_notes_f *	lnk;
  sx_notes_f *	sys;
  sx_notes_f *	img;
  sx_notes_f *	cpd;
  sx_notes_f *	lem;
  sx_unicode_f *	uni;
  FILE *fp;
  const char *fname;
};

struct noset_tab
{
  const char *name;
  const char *one;
};

struct numvmap_tab
{
  const char *name;
  const char *asif;
};

struct sx_config
{
  const char *project;  /* set from -P[PROJECT] */
  const char *signlist; /* default: csl */
  const char *domain; 	/* default: sl */
  const char *font;	/* default: noto */
  const char *merge;	/* default: 00lib/csl.mrg */
};

extern struct sx_config sxconfig;

#define SX_IDATA_COMPONLY (char *)(uintptr_t)-1

extern Hash *parent_sl;
extern const char *parent_sl_project;
extern int unicode_from_parent;
extern int useq_force;

extern int extra_needs, identity_mode, listdef_check;
extern int oid_list, oid_tab;
extern Hash *oids;
extern Hash *oid_sort_keys;

extern char *idata_file;
extern char *idata_type;
extern const char *kdata_file;
extern const char *ldata_file, *ldata_http;
extern const char *gvl_script_type;

extern const char *mergers;

int oid_char_cmp(const void *a, const void *b);
extern int via_tok_cmp(const void *a, const void *b);

extern int cmpstringp(const void *p1, const void *p2);

#if 0
extern void sx_xml(struct sl_signlist *sl);
extern void sx_xml_init(void);
#endif

extern void sx_akas_dump(FILE *fp, struct sl_signlist *sl);
extern void sx_syss_dump(FILE *fp, struct sl_signlist *sl);
extern void sx_compounds(struct sl_signlist *sl);
extern void sx_compound_digests(struct sl_signlist *sl);
extern void sx_homophones(struct sl_signlist *sl);
extern void sx_images(struct sl_signlist *sl);
extern void sx_images_dump(FILE *fp, struct sl_signlist *sl);
extern void sx_inherited(struct sl_signlist *sl);
extern void sx_listdefs(struct sl_signlist *sl, const char *listnames);
extern void sx_listdefs_sort(struct sl_listdef *ldp);
extern void sx_list_dump(FILE *f, struct sl_signlist *sl);
extern void sx_unicode_table(FILE *f, struct sl_signlist *sl);
extern void sx_marshall(struct sl_signlist *sl);
extern void sx_numbers(struct sl_signlist *sl);
extern void sx_qualified(struct sl_signlist *sl);
extern void sx_sortcodes(struct sl_signlist *sl);
extern void sx_unicode(struct sl_signlist *sl);
extern void sx_unicode_p(struct sl_signlist *sl);
extern void sx_values_by_oid(struct sl_signlist *sl);
extern void sx_values_parents(struct sl_signlist *sl);
extern void sx_values_parents_dump(struct sl_signlist *sl);

extern void sx_walk(struct sx_functions *f, struct sl_signlist *sl);

extern struct sx_functions *sx_w_asl_init(FILE *fp, const char *fname);
extern struct sx_functions *sx_w_jsn_init(FILE *fp, const char *fname);
extern struct sx_functions *sx_w_xml_init(FILE *fp, const char *fname);
extern struct sx_functions *sx_w_jox_init(void);
extern struct sx_functions *sx_sll_init(FILE *fp, const char *fname);
extern void sx_s_sll(FILE *f, struct sl_signlist *sl);
extern void sx_oid_list(struct sl_signlist *sl);
extern void sx_oid_tab(struct sl_signlist *sl);

extern void sxx_init(void);
extern void sxx_compound(unsigned const char *c);
extern void sxx_sign(unsigned const char *s);
extern void sxx_output(FILE *fp);

extern void sx_config(const char *project);
extern void sx_merge_subsl(struct sl_signlist *sl);

/* sx_idata.c */
extern void sx_idata_init(struct sl_signlist *sl, const char *idata_file, const char *idata_type);
extern const unsigned char *sx_idata_key(const char *soid, const char *foid, unsigned const char *v);
extern void sx_idata_ctotals(struct sl_signlist *sl);
extern void sx_idata_sign(struct sl_signlist *sl, struct sl_sign *sp);
extern void sx_idata_form_inst(struct sl_signlist *sl, struct sl_inst *fip);
extern void sx_idata_value_inst(struct sl_signlist *sl, struct sl_inst *vip);

extern void sx_ldata_init(struct sl_signlist *sl, const char *ldata_file);
extern void sx_ldata_value_inst(struct sl_signlist *sl, struct sl_inst *vip);

extern void sx_merge(struct sl_signlist *sl);
extern unsigned char *sx_oids_of(struct sl_signlist *sl, unsigned const char *snames);

extern void sx_kdata_init(struct sl_signlist *sl, const char *kdata_file, const char *idata_type);
extern void sx_kdata_componly(struct sl_signlist *sl, const uchar *sname);
extern void sx_kdata_useq(struct sl_signlist *sl);

extern void sx_ldata_sign_inst(struct sl_signlist *sl, struct sl_inst *sip);
extern void sx_ldata_form_inst(struct sl_signlist *sl, struct sl_inst *fip);

extern struct noset_tab *noset (register const char *str, register size_t len);
extern struct numvmap_tab *numvmap (register const char *str, register size_t len);

#endif/*SX_H_*/
