#ifndef GVL_BRIDGE_H_
#define GVL_BRIDGE_H_
extern const char *gvl_bridge(const char *f,size_t l,const unsigned char *g, int sindex);
extern const char *gvl_bridge_oid(void);
extern const char *gvl_bridge_spoid(void);
extern const unsigned char *gvl_bridge_atf2utf(void);
extern const unsigned char *gvl_bridge_signname(void);
extern unsigned const char *gvl_bridge_cuneify(void);
#endif/*GVL_BRIDGE_H_*/
