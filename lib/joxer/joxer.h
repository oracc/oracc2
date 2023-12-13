#ifndef JOXER_
#define JOXER_

type struct rnvval_atts Rats;

extern void (*joxer_ao)(void);
extern void (*joxer_ac)(void);
extern void (*joxer_ch)(Mloc *mp, const char *ch);
extern void (*joxer_ea)(Mloc *mp, const char *pname, Rats *ratts);
extern void (*joxer_ee)(Mloc *mp, const char *pname);
extern void (*joxer_ec)(Mloc *mp, const char *pname, Rats *ratts);
extern void (*joxer_et)(Mloc *mp, const char *pname, Rats *ratts, const char *ch);

extern void jox_jsn_ch(const char *ch);
extern void jox_jsn_ea(const char *pname, Rats *ratts);
extern void jox_jsn_ee(const char *pname);
extern void jox_jsn_ao(void);
extern void jox_jsn_ac(void);

extern void jox_xml_ch(const char *ch);
extern void jox_xml_ea(const char *pname, Rats *ratts);
extern void jox_xml_ee(const char *pname);

extern void joxer_init(struct xnn_data *xdp, const char *rncbase, int val, FILE *xml, FILE *jsn);

#endif/*JOXER_*/
