#include <stdio.h>
#include <assert.h>
#include "rnvif.h"
#include "xmlify.h"
#include "list.h"
#include "mesg.h"
#include "pool.h"
#include "xnn.h"
#include "rnvval.h"
#include "../rnv/erbit.h"
#include "../rnv/m.h"
#include "../rnv/rnv.h"
#include "../rnv/rnx.h"
#include "rnvxml.h"
#include <oraccsys.h>
#include <json.h>
#include "joxer.h"

extern FILE *f_xml;
extern int rnx_n_exp;

struct rnvval_atts *ratts;

static struct pool *xgi_pool;
static List *xgi_stack;
static char xgi_flags[5];

struct xnn_nstab *jsn_xmlns_atts, *xml_xmlns_atts;

static Mloc *ehmp;
#define joxer_mloc(l) ehmp=(l)

#if 0
extern int (*er_printf)(char *format,...);
extern int (*er_vprintf)(char *format,...);
#endif

static void
jox_verror_handler(int erno,va_list ap)
{
#define xvh_err(msg) mesg_averr(ehmp,(msg),ap);
  if(erno&ERBIT_RNL)
    {
      rnl_verror_handler(erno&~ERBIT_RNL,ap);
    }
  else if (erno&ERBIT_RNC)
    {
      rnc_default_verror_handler(erno&!ERBIT_RNC,ap);
    }
  else
    {
      const char *xphase = phase, *xm;
      phase = "rnv";
      switch(erno)
	{
	case RNV_ER_ELEM: xvh_err("element %s^%s not allowed"); break;
	case RNV_ER_AKEY: xvh_err("attribute %s^%s not allowed"); break;
	case RNV_ER_AVAL: xvh_err("attribute %s^%s with invalid value \"%s\""); break;
	case RNV_ER_EMIS: xvh_err("incomplete content"); break;
	case RNV_ER_AMIS: xvh_err("missing attributes of %s^%s"); break;
	case RNV_ER_UFIN: xvh_err("unfinished content of element %s^%s"); break;
	case RNV_ER_TEXT: xvh_err("invalid data or text not allowed"); break;
	case RNV_ER_NOTX: xvh_err("text not allowed"); break;
	default: assert(0);
	}
      xm = rnv_xmsg();
      mesg_append((char*)pool_copy((uccp)xm, xgi_pool));
      free((void*)xm);
      phase = xphase;
    }
}

void (*joxer_ao)(const char *name);
void (*joxer_ac)(void);
void (*joxer_ch)(Mloc *mp, const char *ch);
void (*joxer_ea)(Mloc *mp, const char *pname, Rats *rats);
void (*joxer_ee)(Mloc *mp, const char *pname);
void (*joxer_eaa)(Mloc *mp, const char *pname, Rats *rats);
void (*joxer_eea)(Mloc *mp, const char *pname);
void (*joxer_eaaa)(Mloc *mp, const char *pname, Rats *rats);
void (*joxer_eeaa)(Mloc *mp, const char *pname);
void (*joxer_ec)(Mloc *mp, const char *pname, Rats *rats);
void (*joxer_et)(Mloc *mp, const char *pname, Rats *rats, const char *ch);
void (*joxer_eto)(Mloc *mp, const char *pname, Rats *rats, const char *ch);

static void
joxer_ao_vxj(const char *name)
{
  jox_jsn_ao(name);
}

static void
joxer_ac_vxj(void)
{
  jox_jsn_ac();
}

static void
joxer_ch_vxj(Mloc *mp, const char *ch)
{
  joxer_mloc(mp);
  const char *xch = (ccp)xmlify((ucp)ch);
  rnvval_ch(xch);
  jox_xml_ch(xch);
  jox_jsn_ch(ch);
}

static void
joxer_ea_vxj(Mloc *mp, const char *pname, Rats *rats)
{
  joxer_mloc(mp);
  rnvval_ea(pname, rats);
  jox_xml_ea(pname, rats);
  jox_jsn_ea(pname, rats);
  if (rats)
    {
      rnvval_free_atts(ratts);
      ratts = NULL;
    }
}

static void
joxer_ee_vxj(Mloc *mp, const char *pname)
{
  joxer_mloc(mp);
  rnvval_ee(pname);
  jox_xml_ee(pname);
  jox_jsn_ee(pname);
}

static void
joxer_eaa_vxj(Mloc *mp, const char *pname, Rats *rats)
{
  joxer_mloc(mp);
  rnvval_ea(pname, rats);
  jox_xml_ea(pname, rats);
  jox_jsn_eaa(pname, rats);
  if (rats)
    {
      rnvval_free_atts(ratts);
      ratts = NULL;
    }
}

static void
joxer_eea_vxj(Mloc *mp, const char *pname)
{
  joxer_mloc(mp);
  rnvval_ee(pname);
  jox_xml_ee(pname);
  jox_jsn_eea(pname);
}

static void
joxer_eaaa_vxj(Mloc *mp, const char *pname, Rats *rats)
{
  joxer_mloc(mp);
  rnvval_ea(pname, rats);
  jox_xml_ea(pname, rats);
  jox_jsn_eaaa(pname, rats);
  if (rats)
    {
      rnvval_free_atts(ratts);
      ratts = NULL;
    }
}

static void
joxer_eeaa_vxj(Mloc *mp, const char *pname)
{
  joxer_mloc(mp);
  rnvval_ee(pname);
  jox_xml_ee(pname);
  jox_jsn_eeaa(pname);
}

static void
joxer_ec_vxj(Mloc *mp, const char *pname, Rats *rats)
{
  joxer_ea_vxj(mp, pname, rats);
  joxer_ee_vxj(mp, pname);
}

static void
joxer_et_vxj(Mloc *mp, const char *pname, Rats *rats, const char *ch)
{
  const char *xch = (ccp)xmlify((ucp)ch);
  joxer_mloc(mp);
  rnvval_ea(pname, rats);
  rnvval_ch(xch);
  rnvval_ee(pname);
  jox_xml_ea(pname, rats);
  jox_xml_ch(xch);
  jox_xml_ee(pname);
  jox_jsn_et(pname, rats, ch);
  if (rats)
    {
      rnvval_free_atts(ratts);
      ratts = NULL;
    }
}

static void
joxer_eto_vxj(Mloc *mp, const char *pname, Rats *rats, const char *ch)
{
  const char *xch = (ccp)xmlify((ucp)ch);
  joxer_mloc(mp);
  rnvval_ea(pname, rats);
  rnvval_ch(xch);
  rnvval_ee(pname);
  jox_xml_ea(pname, rats);
  jox_xml_ch(xch);
  jox_xml_ee(pname);
  jox_jsn_eto(pname, rats, ch);
  if (rats)
    {
      rnvval_free_atts(ratts);
      ratts = NULL;
    }
}
static void
joxer_set_vxj(void)
{
  joxer_ch = joxer_ch_vxj;
  joxer_ea = joxer_ea_vxj;
  joxer_ee = joxer_ee_vxj;
  joxer_eaa = joxer_eaa_vxj;
  joxer_eea = joxer_eea_vxj;
  joxer_eaaa = joxer_eaaa_vxj;
  joxer_eeaa = joxer_eeaa_vxj;
  joxer_ec = joxer_ec_vxj;
  joxer_et = joxer_et_vxj;
  joxer_eto = joxer_eto_vxj;
  joxer_ao = joxer_ao_vxj;
  joxer_ac = joxer_ac_vxj;
}

static void
joxer_set_vx(void)
{
  fprintf(stderr, "joxer selection not yet implemented. Stop\n");
  exit(1);
}
static void
joxer_set_vj(void)
{
  fprintf(stderr, "joxer selection not yet implemented. Stop\n");
  exit(1);
}
static void
joxer_set_v(void)
{
  fprintf(stderr, "joxer selection not yet implemented. Stop\n");
  exit(1);
}
static void
joxer_set_xj(void)
{
  fprintf(stderr, "joxer selection not yet implemented. Stop\n");
  exit(1);
}
static void
joxer_set_x(void)
{
  fprintf(stderr, "joxer selection not yet implemented. Stop\n");
  exit(1);
}
static void
joxer_set_j(void)
{
  fprintf(stderr, "joxer selection not yet implemented. Stop\n");
  exit(1);
}

void
joxer_init(struct xnn_data *xdp, const char *rncbase, int val, FILE *xml, FILE *jsn)
{
  int rnv_initialized = 0;
 
  if (val)
    {
      rnvif_init();
      rnv_initialized = 1;
      rnvval_init_err(jox_verror_handler);
      rnvval_init(xdp, rncbase);
      rnv_validate_start();
    }
  
  if (xml)
    {
      if (!rnv_initialized++)
	rnvif_init();
      rnvxml_init_err();
      f_xml = xml;
      xml_xmlns_atts = xdp->nstab;
      xgi_pool = pool_init();
      xgi_stack = list_create(LIST_LIFO);
      xgi_flags[0] = '\0';
    }

  if (jsn)
    {
      jsn_xmlns_atts = xdp->nstab;
      jw_init(jsn);
      /*jw_object_o();*/
    }

  if (val && xml && jsn)
    joxer_set_vxj();
  else if (val && xml)
    joxer_set_vx();
  else if (val && jsn)
    joxer_set_vj();
  else if (val)
    joxer_set_v();
  else if (xml && jsn)
    joxer_set_xj();
  else if (xml)
    joxer_set_x();
  else if (jsn)
    joxer_set_j();
  else
    ; /* no validation or output */
}

void
joxer_term(FILE *xfp, FILE *jfp)
{
  if (jfp)
    {
      /*jw_object_c();*/
      jw_term();
    }
}
