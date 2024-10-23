#ifndef OFP_H_
#define OFP_H_

typedef struct Ofp
{
  Hash *has; /* hash of U+ to name and name to U+ */
  Hash *sst; /* hash of name to sets the name has an entry in */
  Hash *cvt; /* hash of name to character variants the name has */
  Hash *lig; /* hash of name to ligatures in u12345_u12346 format;
		each component has an entry */
} Ofp;

#endif/*OFP_H_*/
