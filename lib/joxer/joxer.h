#ifndef JOXER_
#define JOXER_

#include <memo.h>
#include <xnn.h>
#include <rnvval.h>

typedef struct rnvval_atts Rats;

extern struct xnn_nstab *jsn_xmlns_atts, *xml_xmlns_atts;

extern void (*joxer_ao)(const char *name);
extern void (*joxer_ac)(void);
extern void (*joxer_ch)(Mloc *mp, const char *ch);
extern void (*joxer_ea)(Mloc *mp, const char *pname, Rats *ratts);
extern void (*joxer_ee)(Mloc *mp, const char *pname);
extern void (*joxer_eaa)(Mloc *mp, const char *pname, Rats *ratts);
extern void (*joxer_eea)(Mloc *mp, const char *pname);
extern void (*joxer_eaaa)(Mloc *mp, const char *pname, Rats *ratts);
extern void (*joxer_eeaa)(Mloc *mp, const char *pname);
extern void (*joxer_ec)(Mloc *mp, const char *pname, Rats *ratts);
extern void (*joxer_et)(Mloc *mp, const char *pname, Rats *ratts, const char *ch);
extern void (*joxer_eto)(Mloc *mp, const char *pname, Rats *ratts, const char *ch);

extern void jox_jsn_ch(const char *ch);
extern void jox_jsn_ea(const char *pname, Rats *ratts);
extern void jox_jsn_ee(const char *pname);
extern void jox_jsn_eaa(const char *pname, Rats *ratts);
extern void jox_jsn_eea(const char *pname);
extern void jox_jsn_eaaa(const char *pname, Rats *ratts);
extern void jox_jsn_eeaa(const char *pname);
extern void jox_jsn_ao(const char *name);
extern void jox_jsn_ac(void);
extern void jox_jsn_et(const char *pname, Rats *ratts, const char *ch);
extern void jox_jsn_eto(const char *pname, Rats *ratts, const char *ch);

extern void jox_xml_ch(const char *ch);
extern void jox_xml_ea(const char *pname, Rats *ratts);
extern void jox_xml_ee(const char *pname);

extern void joxer_init(struct xnn_data *xdp, const char *rncbase, int val, FILE *xml, FILE *jsn);
extern void joxer_term(FILE *xfp, FILE *jfp);

extern void jox_jsn_output(FILE *fp);
extern void jox_xml_output(FILE *fp);
extern void jox_xml_inl(char *ch);

extern void joxer_set_inl(int i);

#endif/*JOXER_*/
