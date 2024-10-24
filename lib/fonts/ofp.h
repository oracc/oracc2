#ifndef OFP_H_
#define OFP_H_

/* When indexing by U+ or u123AF names we use the 5-hex-digit portion
   only for all hash keys */
typedef struct Ofp
{
  const char *name;
  const char *file;
  int nglyphs;
  struct Ofp_glyph *glyphs;
  Pool *p;
} Ofp;

typedef enum Ofp_feature
  {
    OFPF_BASE , OFPF_LIGA , OFPF_SSET , OFPF_CVNN , OFPF_OIVS , OFPF_SALT , OFPF_NONE
  } Ofp_feature;

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
  const char *ivs;
} Ofp_glyph;

#if 0
enum ofp_features { OFP_HAS_SSET=0x01, OFP_HAS_CVNN=0x02, OFP_HAS_SALT=0x04, OFP_HAS_OIVS=0x08 };
typedef struct ofp_has   { const char *name; const char *key; unsigned char features[1]; } ofp_has;
typedef struct ofp_ssets { unsigned char bv[4] ; } ofp_ssets; /* max 20 per OpenType design */
typedef struct ofp_cvnns { unsigned char bv[13]; } ofp_cvnns; /* max 99 per OpenType design */
typedef struct ofp_salts { unsigned char bv[32]; } ofp_salts; /* not sure what limit is yet; this allows 256 */
typedef struct ofp_oivs  { unsigned char bv[32]; } ofp_oivs;  /* max 239 based on Unicode IVS E01EF - E0100 */ 
enum ofp_type { OFP_NONE , OFP_SALT , OFP_SSET , OFP_CVNN };
#endif

typedef struct ofp_bv_arg { Ofp *o; const char *code; FILE *fp; } ofp_bv_arg;

extern Ofp *ofp_init(void);
extern void ofp_term(Ofp *ofp);
extern Ofp *ofp_load(const char *font);
extern void ofp_dump(Ofp *ofp, FILE *fp);

#endif/*OFP_H_*/
