#ifndef OFP_H_
#define OFP_H_

typedef struct Ofp
{
  Hash *has; /* hash of U+ to name and name to U+ */
  Hash *sst; /* hash of name to sets the name has an entry in */
  Hash *cvt; /* hash of name to character variants the name has */
  Hash *slt; /* hash of name to salts the name has */
  Hash *oiv; /* hash of name to IVS the name can be used with; derived
		from ligature data */
  Hash *lig; /* hash of initial component of ligature in u12345_u12346
		format. Value is another hash, giving ligature to
		name:
		lig{u12000} => { u12000_u12000 => AA_liga }
	      */
  Pool *p;
} Ofp;

typedef struct ofp_ssets { unsigned char bv[4] ; } ofp_ssets; /* max 20 per OpenType design */
typedef struct ofp_cvnns { unsigned char bv[13]; } ofp_cvnns; /* max 99 per OpenType design */
typedef struct ofp_salts { unsigned char bv[32]; } ofp_salts; /* not sure what limit is yet; this allows 256 */
typedef struct ofp_oivs  { unsigned char bv[32]; } ofp_oivs;  /* max 239 based on Unicode IVS E01EF - E0100 */ 

enum ofp_type { OFP_NONE , OFP_SALT , OFP_SSET , OFP_CVNN };

extern Ofp *ofp_init(void);
extern Ofp *ofp_load(const char *font);

#endif/*OFP_H_*/
