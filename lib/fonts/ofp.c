#include <oraccsys.h>
#include "ofp.h"

static enum ofp_type ofp_ext_type(const char *ext);
static void ofp_set_salt(Ofp *ofp, ofp_has *h, const char *name);
static void ofp_set_sset(Ofp *ofp, ofp_has *h, const char *name);
static void ofp_set_cvnn(Ofp *ofp, ofp_has *h, const char *name);
static void ofp_set_oiv(Ofp *ofp,  ofp_has *h, const char *name);

/* int is the bit-index; void* is user data */
typedef void (bv_each_fnc)(int,void*);

static void
bv_set(unsigned char *bv, int nth)
{
  int i = nth/8;
  int b = nth%8;
  if (b)
    bv[i] |= 1<<(b-1);
  else
    bv[i-1] |= 1<<7;
}

static void
bv_each(unsigned char *bv, int len, bv_each_fnc f, void *user)
{
  int i;
  for (i = 0; i < len; ++i)
    if (bv[i])
      {
	int j;
	for (j = 0; j < 8; ++j)
	  if (bit_get(bv[i], (1<<j)))
	    f((i*8)+j+1, user);
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

static const char *
un_u(const char *u)
{
  if ('u' == *u)
    {
      const char *h = u+1;
      while (isxdigit(*h))
	++h;
      if ('\0' == *h || '.' == *h)
	return u+1;
    }
  return u;
}

static const char *
ucode_from_name(Ofp *o, const char *name)
{
  const char *code = un_u(name);
  if (code != name)
    {
      char buf[strlen(name)];
      strcpy(buf, code);
      char *dot = strchr(buf, '.');
      if (dot)
	*dot = '\0';
      if ((code = (ccp)hash_exists(o->has, (uccp)buf)))
	return code;
      else
	return (ccp)pool_copy((uccp)buf, o->p);
    }
  return NULL;
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
      unsigned char *fmem, **lp;
      size_t nline;

      ofp = ofp_init();
      ofp->name = fontname;
      ofp->file = (ccp)pool_copy((uccp)buf, ofp->p);
      lp = loadfile_lines3((uccp)buf, &nline, &fmem);
      int i;
      for (i = 0; i < nline; ++i)
	{
	  ofp_has *has = NULL;
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
	      char *ivs = strstr(liga, "_uE01");
	      if (ivs && (has = hash_find(ofp->has,(uccp)lig)))
		ofp_set_oiv(ofp, has, ivs+2);
	    }
	  else
	    {
	      char *ucode = strchr((ccp)lp[i], '\t');
	      if (ucode)
		{
		  char *name = (char*)lp[i];
		  *ucode++ = '\0';
		  ucode += 2;
		  if (strcmp(ucode, "0000") || (ucode = (char*)ucode_from_name(ofp, name)))
		    {
		      has = hash_find(ofp->has, (uccp)ucode);
		      if (!has)
			{
			  hash_add(ofp->has, (uccp)ucode, (has = memo_new(ofp->m_has)));
			  has->name = name;
			  has->key = un_u(name);
			}
		    }
		  else
		    {
		      if (!(has = hash_find(ofp->zero, (uccp)un_u(name))))
			{
			  hash_add(ofp->zero, (uccp)un_u(name), (has = memo_new(ofp->m_has)));
			  has->name = name;
			  has->key = un_u(name);
			}
		    }
		  char *dot = strchr(name, '.');
		  if (dot)
		    {
		      *dot++ = '\0';
		      switch (ofp_ext_type(dot))
			{
			case OFP_SALT:
			  ofp_set_salt(ofp, has, dot);
			  break;
			case OFP_SSET:
			  ofp_set_sset(ofp, has, dot);
			  break;
			case OFP_CVNN:
			  ofp_set_cvnn(ofp, has, dot);
			  break;
			default:
			  /* ignore extensions that don't have known semantics */
			  break;
			}
		    }
		}
	      else
		{
		  fprintf(stderr, "%s:%d: malformed name\tucode line\n", buf, i+1);
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
  ofp->zero = hash_create(128);
  ofp->sst = hash_create(128);
  ofp->cvt = hash_create(128);
  ofp->slt = hash_create(128);
  ofp->lig = hash_create(128);
  ofp->oiv = hash_create(128);
  ofp->p = pool_init();
  ofp->m_has = memo_init(sizeof(ofp_has), 256);
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

  if ('c' == ext[0] && 'v' == ext[1] && isdigit(ext[2]) && isdigit(ext[3]) && !ext[4])
    return OFP_CVNN;

  if ('s' == ext[0] && 's' == ext[1] && isdigit(ext[2]) && isdigit(ext[3]) && !ext[4])
    return OFP_SSET;

  return OFP_NONE;
}

static void
ofp_set_salt(Ofp *ofp, ofp_has *h, const char *ext)
{
  ofp_salts *o = hash_find(ofp->slt, (uccp)h->key);
  if (!o)
    hash_add(ofp->slt, (uccp)h->key, (o = calloc(1, sizeof(ofp_salts))));
  bv_set(o->bv, atoi(ext));
  bit_set(h->features[0], OFP_HAS_SALT);
}

static void
ofp_set_sset(Ofp *ofp, ofp_has *h, const char *ext)
{
  ofp_ssets *o = hash_find(ofp->sst, (uccp)h->key);
  if (!o)
    hash_add(ofp->sst, (uccp)h->key, (o = calloc(1, sizeof(ofp_ssets))));
  bv_set(o->bv, atoi(ext+2));
  bit_set(h->features[0], OFP_HAS_SSET);
}

static void
ofp_set_cvnn(Ofp *ofp, ofp_has *h, const char *ext)
{
  ofp_cvnns *o = hash_find(ofp->cvt, (uccp)h->key);
  if (!o)
    hash_add(ofp->cvt, (uccp)h->key, (o = calloc(1, sizeof(ofp_cvnns))));
  bv_set(o->bv, atoi(ext+2));
  bit_set(h->features[0], OFP_HAS_CVNN);
}

static void
ofp_set_oiv(Ofp *ofp, ofp_has *h, const char *ext)
{
  ofp_oivs *o = hash_find(ofp->oiv, (uccp)h->key);
  if (!o)
    hash_add(ofp->oiv, (uccp)h->key, (o = calloc(1, sizeof(ofp_oivs))));
  bv_set(o->bv, strtol(ext, NULL, 16));
  bit_set(h->features[0], OFP_HAS_OIVS);
}

static void
ofp_xml_sset(int f, void *arg)
{
  fprintf(((ofp_bv_arg*)arg)->fp, "<ss>ss%02d</ss>", f);
}

static void
ofp_xml_cvnn(int f, void *arg)
{
  fprintf(((ofp_bv_arg*)arg)->fp, "<cv>cv%02d</cv>", f);
}

static void
ofp_xml_salt(int f, void *arg)
{
  fprintf(((ofp_bv_arg*)arg)->fp, "<s>%d</s>", f);
}

void
ofp_xml_feature(int f, void *arg)
{
  /* The feature bv is going to be switched by an enum which is keyed
     to the bit index value, not the bit index itself so we have to
     recalculate from the index to the value */
  switch (1<<(f-1))
    {
    case OFP_HAS_SSET:
      {
	ofp_ssets *s = hash_find(((ofp_bv_arg*)arg)->o->sst, (uccp)((ofp_bv_arg*)arg)->code);
	if (s)
	  {
	    fprintf(((ofp_bv_arg*)arg)->fp, "<ssets>");
	    bv_each(s->bv, sizeof(s), ofp_xml_sset, arg);
	    fprintf(((ofp_bv_arg*)arg)->fp, "</ssets>");
	  }
      }
      break;
    case OFP_HAS_CVNN:
      {
	ofp_cvnns *s = hash_find(((ofp_bv_arg*)arg)->o->cvt, (uccp)((ofp_bv_arg*)arg)->code);
	if (s)
	  {
	    fprintf(((ofp_bv_arg*)arg)->fp, "<cvnns>");
	    bv_each(s->bv, sizeof(s), ofp_xml_cvnn, arg);
	    fprintf(((ofp_bv_arg*)arg)->fp, "</cvnns>");
	  }
      }
      break;
    case OFP_HAS_SALT:
      {
	ofp_salts *s = hash_find(((ofp_bv_arg*)arg)->o->slt, (uccp)((ofp_bv_arg*)arg)->code);
	if (s)
	  {
	    fprintf(((ofp_bv_arg*)arg)->fp, "<salts>");
	    bv_each(s->bv, sizeof(s), ofp_xml_salt, arg);
	    fprintf(((ofp_bv_arg*)arg)->fp, "</salts>");
	  }
      }
      break;
#if 0
    case OFP_HAS_OIVS:
      bv_each(bv, ofp_xml_oivs, arg);
      break;
#endif
    default:
      break;
    }
}

void
ofp_xml(Ofp *ofp, FILE *fp)
{
  ofp_bv_arg o = { ofp , NULL, fp };
  fprintf(fp, "<ofp n=\"%s\" f=\"%s\">", ofp->name, ofp->file);
  const char **k = hash_keys(ofp->has);
  int i;
  for (i = 0; k[i]; ++i)
    {
      ofp_has *has = hash_find(ofp->has, (uccp)k[i]);
      fprintf(fp, "<has u=\"%s\" n=\"%s\">", k[i], has->name);
      if (has->features)
	{
	  o.code = k[i];
	  bv_each(has->features, sizeof(has->features), ofp_xml_feature, &o);
	}
      fprintf(fp, "</has>");
    }
  fprintf(fp, "</ofp>");
}
