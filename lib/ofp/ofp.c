#include <oraccsys.h>
#include "osl_unicode.h"
#include "ofp.h"

const char *ofp_feat_str[OFPF_NONE];

/* The OFP file format is very simple and consists of two different kinds of lines:

   1) NAME\tU+CODE

   2) NAME '<-' LIGATURE_SEQUENCE_MAPPING_TO_NAME

   Examples of (1):

   u12000	U+12000
   u122DC.ss01	U+0000
   GEME2.liga	U+0000
   u12324.1	U+0000
   u12324.cv01	U+0000
   u12324_uE0100	U+0000

   Examples of (2):

   u12016_uni200D_u122AE <- u12016_uni200D_u122AE 
   u1202D_uni200D_u12097 <- u1202D_uni200D_u12097 

   NUMUN2.liga <- u12364_u121B8 
   PAD3.liga <- u12146_u12292 

 */

static int
ofp_glyph_cmp(const void *a, const void *b)
{
#define ap ((Ofp_glyph*)a)
#define bp ((Ofp_glyph*)b)

  int ret;

  if (ap->key && bp->key)
    {
      ret = strcmp(ap->key, bp->key);
      if (ret)
	return ret;
      
      if ((ret = (int)ap->f - (int)bp->f))
	return ret;
      
      if ((ret = ap->f_int - bp->f_int))
	return ret;

      if (ap->liga && bp->liga && (ret = strcmp(ap->liga,bp->liga)))
	return ret;
      else if (ap->liga)
	return 1;
      else if (bp->liga)
	return -1;
      
      return strcmp(ap->name, bp->name);
    }
  else
    {
      if (ap->key)
	return -1;
      else
	return 1;
    }
}

Ofp *
ofp_init(void)
{
  Ofp *ofp = calloc(1, sizeof(Ofp));
  ofp->m_sign = memo_init(sizeof(Ofp_sign), 256);
  ofp->m_liga = memo_init(sizeof(Ofp_liga), 128);
  ofp->h_sign = hash_create(2048);
  ofp->h_liga = hash_create(128);
  ofp->p = pool_init();
  ofp->osl = osl_unicode();
  ofp_feat_str[OFPF_BASE] = "";
  ofp_feat_str[OFPF_LIGA] = "liga";
  ofp_feat_str[OFPF_SSET] = "ss";
  ofp_feat_str[OFPF_CVNN] = "cv";
  ofp_feat_str[OFPF_SALT] = "salt";
  ofp_feat_str[OFPF_NONE] = "?";
  return ofp;
}

Ofp *
ofp_load(const char *fontname)
{
  Ofp *ofp = NULL;
  
  char buf[strlen(oracc())+strlen("/lib/data/.ofp0")+strlen(fontname)];
  if (strcmp(fontname, "-"))
    sprintf(buf, "%s/lib/data/%s.ofp", oracc(), fontname);
  else
    strcpy(buf, "-");

  if (!access(buf, R_OK) || !strcmp(buf, "-"))
    {
      ofp = ofp_init();
      ofp->name = fontname;
      ofp->file = (ccp)pool_copy((uccp)buf, ofp->p);
      ofp_ingest(ofp);
      qsort(ofp->glyphs, ofp->nglyphs, sizeof(Ofp_glyph), ofp_glyph_cmp);
      ofp_marshall(ofp);
   }
  else
    fprintf(stderr, "ofp_load: unable to read %s: %s\n", buf, strerror(errno));
  return ofp;
}
