#ifndef GX_H_
#define GX_H_ 1

#include <rnvxml.h>
#include <cbd.h>

typedef void (*iterator_fnc)(void*);

extern Hash *cbds;

extern int check;
extern int entries;
extern int keepgoing;
extern int sigs;

extern int lnum;
extern int verbose;
extern int out_stdout;

extern const char *file;

extern FILE *f_xml;

enum o_mode { O_JSN , O_XML };

#if 0
extern void o_tg2(struct cbd*c);
#endif

extern void o_jox(struct cbd*c);
extern void o_jsn(struct cbd*c);

extern Ratts *ratts_cbd(Cbd *c, enum o_mode mode);
extern Ratts *ratts_cpd(Cgp *c, enum o_mode mode);
extern Ratts *ratts_entry(Entry *e, enum o_mode mode);
extern Ratts *ratts_form(Field *f, enum o_mode mode);
extern Ratts *ratts_norm(Field *f, enum o_mode mode);
extern void ratts_kis(List *lp, Kis_data k);
extern Ratts *ratts_list2ratts(List *lp);
extern Ratts *ratts_nmfm(Field *f, enum o_mode mode);
extern Ratts *ratts_one(const char *attr, const char *aval);

extern void validator(struct cbd*c);

extern void common_init(void);
extern void common_term(void);
extern void untab(unsigned char *s);
extern unsigned char *tok(unsigned char *s, unsigned char *end);
extern unsigned char *check_bom(unsigned char *s);
extern unsigned char **setup_lines(unsigned char *ftext);
extern unsigned char *slurp(const char *caller, const char *fname, ssize_t *fsize);
extern List*cof_sigs(Cform *f2p, Pool *p);
extern void psu_sigs(Entry *ep);
extern void lemm_sigs(const char *fn);

#endif/*GX_H_*/
