#include <oraccsys.h>
#include "ofp.h"

static enum ofp_type ofp_ext_type(const char *ext);
static void ofp_set_salt(Ofp *ofp, const char *n, const char *name);
static void ofp_set_sset(Ofp *ofp, const char *n, const char *name);
static void ofp_set_cvnn(Ofp *ofp, const char *n, const char *name);

typedef void (bv_each_fnc)(int);

static void
bv_set(unsigned char *bv, int nth)
{
  bit_set(bv[nth/8], nth%8);
}

static void
bv_each(unsigned char *bv, int len, bv_each_fnc f)
{
  int i;
  for (i = 0; i < len; ++i)
    if (bv[i])
      {
	int j;
	for (j = 0; j < 8; ++j)
	  if (bit_get(bv[i], j))
	    f((i*8)+j);
      }
}

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
      unsigned char *fmem, **lp;
      size_t nline;

      ofp = ofp_init();
      
      lp = loadfile_lines3((uccp)buf, &nline, &fmem);
      int i;
      for (i = 0; i < nline; ++i)
	{
	  char *liga = strstr((ccp)lp[i], "<-");
	  if (liga)
	    {
	      char *e = liga;
	      while (isspace(e[-1]))
		--e;
	      e[-1] = '\0';
	      liga += 2;
	      while (isspace(*liga))
		++liga;
	      char lig[strlen(liga)], *l;
	      strcpy(lig, liga);
	      l = strchr(lig, '_');
	      *l = '\0';
	      Hash *lhash = hash_find(ofp->lig, (uccp)lig);
	      if (lhash)
		hash_add(ofp->lig, (uccp)liga, lp);
	      else
		{
		  hash_add(ofp->lig, pool_copy((uccp)lig, ofp->p), (lhash = hash_create(20)));
		  hash_add(lhash, (uccp)liga, lp);
		}
	    }
	  else
	    {
	      char *ucode = strchr((ccp)lp[i], '\t');
	      if (ucode)
		{
		  char *name = (char*)lp[i];
		  ++ucode;
		  hash_add(ofp->has, (uccp)name, ucode);
		  if (strcmp(ucode, "U+0000"))
		    hash_add(ofp->has, (uccp)ucode, name);
		  char *dot = strchr(name, '.');
		  if (dot)
		    {
		      switch (ofp_ext_type(++dot))
			{
			case OFP_SALT:
			  ofp_set_salt(ofp, name, dot);
			  break;
			case OFP_SSET:
			  ofp_set_sset(ofp, name, dot);
			  break;
			case OFP_CVNN:
			  ofp_set_cvnn(ofp, name, dot);
			  break;
			default:
			  /* ignore extensions that don't have known semantics */
			  break;
			}
		    }
		}
	      else
		{
		  fprintf(stderr, "%s:%d: malformed name\tucode line\n", buf, i);
		}
	    }
	}
    }
  else
    fprintf(stderr, "ofp_load: unable to read %s: %s\n", buf, strerror(errno));
  return ofp;
}

Ofp *
ofp_init(void)
{
  Ofp *ofp = calloc(1, sizeof(Ofp));
  ofp->has = hash_create(2048);
  ofp->sst = hash_create(128);
  ofp->cvt = hash_create(128);
  ofp->slt = hash_create(128);
  ofp->lig = hash_create(128);
  ofp->p = pool_init();
  return ofp;
}

enum ofp_type
ofp_ext_type(const char *ext)
{
  if (isdigit(*ext))
    {
      do
	++ext;
      while (isdigit(*ext));
      if (!*ext)
	return OFP_SALT;
    }

  if ('c' == ext[0] && 'c' == ext[1] && isdigit(ext[2]) && isdigit(ext[3]) && !ext[4])
    return OFP_SSET;

  if ('s' == ext[0] && 's' == ext[1] && isdigit(ext[2]) && isdigit(ext[3]) && !ext[4])
    return OFP_CVNN;

  return OFP_NONE;
}

static void
ofp_set_salt(Ofp *ofp, const char *n, const char *name)
{
  ofp_salts *o = hash_find(ofp->slt, (uccp)name);
  if (!o)
    hash_add(ofp->slt, (uccp)name, (o = calloc(1, sizeof(ofp_salts))));
  bv_set(o->bv, atoi(n));
}

static void
ofp_set_sset(Ofp *ofp, const char *n, const char *name)
{
  ofp_ssets *o = hash_find(ofp->sst, (uccp)name);
  if (!o)
    hash_add(ofp->sst, (uccp)name, (o = calloc(1, sizeof(ofp_ssets))));
  bv_set(o->bv, atoi(n));
}

static void
ofp_set_cvnn(Ofp *ofp, const char *n, const char *name)
{
  ofp_cvnns *o = hash_find(ofp->cvt, (uccp)name);
  if (!o)
    hash_add(ofp->cvt, (uccp)name, (o = calloc(1, sizeof(ofp_cvnns))));
  bv_set(o->bv, atoi(n));
}
