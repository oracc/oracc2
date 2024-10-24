#ifndef OFP_H_
#define OFP_H_

/* When indexing by U+ or u123AF names we use the 5-hex-digit portion
   only for all hash keys */
typedef struct Ofp
{
  const char *name;
  const char *file;
  Hash *has; /* hash of U+ to ofp_has */
  Hash *zero;/* hash of names which map to U+0000 */
  Hash *sst; /* hash of name to sets the name has an entry in */
  Hash *cvt; /* hash of name to character variants the name has */
  Hash *slt; /* hash of name to salts the name has */
  Hash *oiv; /* hash of name to IVS the name can be used with; derived
		from ligature data */
  Hash *lig; /* hash of initial component of ligature in u12345_u12346
		format. Value is another hash, giving ligature to
		name:
		lig{12000} => { u12000_u12000 => AA_liga }
	      */
  Pool *p;
  Memo *m_has;
} Ofp;

enum ofp_features { OFP_HAS_SSET=0x01, OFP_HAS_CVNN=0x02, OFP_HAS_SALT=0x04, OFP_HAS_OIVS=0x08 };

typedef struct ofp_has   { const char *name; const char *key; unsigned char features[1]; } ofp_has;
typedef struct ofp_ssets { unsigned char bv[4] ; } ofp_ssets; /* max 20 per OpenType design */
typedef struct ofp_cvnns { unsigned char bv[13]; } ofp_cvnns; /* max 99 per OpenType design */
typedef struct ofp_salts { unsigned char bv[32]; } ofp_salts; /* not sure what limit is yet; this allows 256 */
typedef struct ofp_oivs  { unsigned char bv[32]; } ofp_oivs;  /* max 239 based on Unicode IVS E01EF - E0100 */ 

typedef struct ofp_bv_arg { Ofp *o; const char *code; FILE *fp; } ofp_bv_arg;

enum ofp_type { OFP_NONE , OFP_SALT , OFP_SSET , OFP_CVNN };

extern Ofp *ofp_init(void);
extern void ofp_term(Ofp *ofp);
extern Ofp *ofp_load(const char *font);
extern void ofp_xml(Ofp *ofp, FILE *fp);

#endif/*OFP_H_*/
