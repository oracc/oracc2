#include <oraccsys.h>
#include "ofp.h"

static enum Ofp_feature ofp_ext_type(const char *ext);
static const char *un_u(const char *u);
static void ofp_ingest(Ofp *ofp);
static int fnum(const char *n);

static const char *ofp_feat_str[OFPF_NONE];

#if 0
static void ofp_set_salt(Ofp *ofp, ofp_has *h, const char *name);
static void ofp_set_sset(Ofp *ofp, ofp_has *h, const char *name);
static void ofp_set_cvnn(Ofp *ofp, ofp_has *h, const char *name);
static void ofp_set_oiv(Ofp *ofp,  ofp_has *h, const char *name);
#endif

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

static void
ofp_set_glyph(Ofp *o, int i, const char *name, const char *code, const char *fcode,
	      Ofp_feature f, const char *ext, const char *ligl, const char *liga, const char *ivs)
{
#define gp(i) (&o->glyphs[i])
  gp(i)->index = i;
  gp(i)->key = un_u(ligl?ligl:name);
  gp(i)->name = name;
  gp(i)->code = code;
  gp(i)->fcode = fcode;
  gp(i)->f = f;
  if (ext)
    {
      gp(i)->f_int = (f == OFPF_OIVS ? (int)strtol(ext,NULL,16) : fnum(ext));
      gp(i)->f_chr = ext;
    }
  gp(i)->ligl = ligl;
  gp(i)->liga = liga;
  gp(i)->ivs = ivs;
#undef gp
}

static int
fnum(const char *n)
{
  while (*n && !isdigit(*n))
    ++n;
  if (n)
    return atoi(n);
  else
    return 0;
}

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
      return (ccp)pool_copy((uccp)buf, o->p);
    }
  return NULL;
}

static int
ofp_glyph_cmp(const void *a, const void *b)
{
#define ap ((Ofp_glyph*)a)
#define bp ((Ofp_glyph*)b)

  int ret;
  
  ret = strcmp(ap->key, bp->key);
  if (ret)
    return ret;

  if ((ret = (int)ap->f - (int)bp->f))
    return ret;

  if ((ret = ap->f_int - bp->f_int))
    return ret;

  return strcmp(ap->name, bp->name);
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
   }
  else
    fprintf(stderr, "ofp_load: unable to read %s: %s\n", buf, strerror(errno));
  return ofp;
}

static void
ofp_ingest(Ofp *ofp)
{
  unsigned char *fmem, **lp;
  size_t nline;

  lp = loadfile_lines3((uccp)ofp->file, &nline, &fmem);

  ofp->nglyphs = nline;
  ofp->glyphs = calloc(ofp->nglyphs, sizeof(Ofp_glyph));
  
  int i;
  for (i = 0; i < nline; ++i)
    {
      char *name = (char*)lp[i];
      Ofp_feature f = OFPF_BASE;
      const char *ext = NULL;
      char *liga = strstr((ccp)lp[i], "<-");
      char *ligl = NULL;
      const char *fcode = NULL, *ucode = NULL;
      const char *ivs = NULL;
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
	  ligl = (char*)pool_copy((uccp)lig, ofp->p);

	  /* Now lp is the name glyph name (may have double
	     extension, e.g., AGRIG.liga.ss01); lig is the lead
	     ligature element; liga is the ligature sequence */
	  char *dot = strrchr((ccp)lp, '.');
	  if (dot && strcmp(dot, ".liga")) /* .liga is left as part of the name */
	    {
	      *dot++ = '\0';
	      ext = dot;
	      f = ofp_ext_type(dot);
	    }

	  if ((ivs = strstr(liga, "_uE01")))
	    ivs += 2;
	}
      else
	{
	  char *ucode = strchr((ccp)lp[i], '\t');
	  if (ucode)
	    {
	      *ucode++ = '\0';
	      ucode += 2;
	      fcode = ucode;
	      if (!strcmp(ucode, "0000"))
		ucode = (char*)ucode_from_name(ofp, name);
	      char *dot = strchr(name, '.');
	      if (dot)
		{
		  *dot++ = '\0';
		  ext = dot;
		  f = ofp_ext_type(dot);
		}
	    }
	  else
	    {
	      fprintf(stderr, "%s:%d: malformed name\tucode line\n", ofp->file, i+1);
	      continue;
	    }
	}
      ofp_set_glyph(ofp, i, name, ucode, fcode, f, ext, ligl, liga, ivs);
    }
}

void
ofp_dump(Ofp *o, FILE *fp)
{
#define nonull(s) (s ? s : "")
#define gp(i) (&o->glyphs[i])
  int i;
  for (i = 0; i < o->nglyphs; ++i)
    {
      fprintf(fp,
	      "%s\t%s\t%s\t%s\t"
	      "%s\t%s\t%d\t"
	      "%s\t%s\t%s\n",
	      gp(i)->key, gp(i)->name, nonull(gp(i)->code), nonull(gp(i)->fcode),
	      ofp_feat_str[gp(i)->f], nonull(gp(i)->f_chr), gp(i)->f_int,
	      nonull(gp(i)->ligl), nonull(gp(i)->liga), nonull(gp(i)->ivs)
	      );
    }
#undef gp
}

Ofp *
ofp_init(void)
{
  Ofp *ofp = calloc(1, sizeof(Ofp));
  ofp->p = pool_init();
  ofp_feat_str[OFPF_BASE] = "";
  ofp_feat_str[OFPF_LIGA] = "liga";
  ofp_feat_str[OFPF_SSET] = "ss";
  ofp_feat_str[OFPF_CVNN] = "cv";
  ofp_feat_str[OFPF_SALT] = "salt";
  ofp_feat_str[OFPF_NONE] = "?";
  return ofp;
}

enum Ofp_feature
ofp_ext_type(const char *ext)
{
  if (isdigit(*ext))
    {
      do
	++ext;
      while (isdigit(*ext));
      if (!*ext)
	return OFPF_SALT;
    }

  if ('c' == ext[0] && 'v' == ext[1] && isdigit(ext[2]) && isdigit(ext[3]) && !ext[4])
    return OFPF_CVNN;

  if ('s' == ext[0] && 's' == ext[1] && isdigit(ext[2]) && isdigit(ext[3]) && !ext[4])
    return OFPF_SSET;

  return OFPF_NONE;
}

#if 0
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
#endif
