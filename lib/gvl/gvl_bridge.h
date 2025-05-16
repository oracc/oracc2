#ifndef GVL_BRIDGE_H_
#define GVL_BRIDGE_H_
struct oiv_data
{
  const char *code;
  const char *sset;
  const char *salt;
  const char *oivs;
};

extern struct oiv_data *gvl_get_script(const char *ucode);
extern void gvl_set_script(const char *scripttype);
extern const char *gvl_bridge(const char *f,size_t l,const unsigned char *g, int sindex);
extern const char *gvl_bridge_key(void);
extern const char *gvl_bridge_oid(void);
extern const char *gvl_bridge_spoid(void);
extern const char *gvl_bridge_ucode(void);
extern const unsigned char *gvl_bridge_atf2utf(void);
extern const unsigned char *gvl_bridge_signname(void);
extern unsigned const char *gvl_bridge_cuneify(void);
extern unsigned const char *gvl_bridge_oid_name(const char *oid);
extern unsigned const char *gvl_bridge_spoid_name(const char *oid);
const unsigned char *gvl_bridge_seq(const char *oid);
#endif/*GVL_BRIDGE_H_*/
