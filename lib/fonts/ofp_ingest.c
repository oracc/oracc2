#include <oraccsys.h>
#include "ofp.h"

static enum Ofp_feature ext_type(const char *ext);
static int fnum(const char *n);
static void set_glyph(Ofp *o, int i, const char *name, const char *code, const char *fcode,
		      Ofp_feature f, const char *ext, const char *ligl, const char *liga, const char *ivs);
static const char *ucode_from_name(Ofp *o, const char *name);
static const char *un_u(const char *u);
static int unicode_value(const char *u);

void
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
	  *e = '\0';
	  liga += 2;
	  while (isspace(*liga))
	    ++liga;
	  hash_add(ofp->h_liga, (uccp)name, &ofp->glyphs[i]);
	  char lig[strlen(liga)], *l;
	  strcpy(lig, liga);
	  l = strchr(lig, '_');
	  *l = '\0';
	  ligl = (char*)pool_copy((uccp)lig, ofp->p);

	  /* Now lp is the glyph name (may have double
	     extension, e.g., AGRIG.liga.ss01); lig is the lead
	     ligature element; liga is the ligature sequence */
	  char *dot = strrchr((ccp)lp, '.');
	  if (dot && strcmp(dot, ".liga")) /* .liga is left as part of the name */
	    {
	      *dot++ = '\0';
	      ext = dot;
	      f = ext_type(dot);
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
	      /* ignore entries that have ucode=0 unless they are .liga + features */
	      if (!strcmp(ucode, "0000"))
		{
		  if (strstr(name, ".liga"))
		    {
		      /* handle AGRIG.liga.ss01 */
		      char *dot = strrchr(name, '.');
		      if (strcmp(dot+1, "liga"))
			{
			  *dot++ = '\0';
			  Ofp_glyph *gp = hash_find(ofp->h_liga, (uccp)name);
			  if (gp)
			    {
			      ligl = (char*)gp->ligl;
			      liga = (char*)gp->liga;
			      ucode = (char*)gp->code;
			      ext = dot;
			      f = ext_type(dot);
			    }
			}
		      else
			name = "-";
		    }
		  else
		    {
		      ucode = (char*)ucode_from_name(ofp, name);
		      if (!unicode_value(ucode))
			{
			  name = "-";
			  ucode = "0000";
			}
		    }
		}
	      if (!ligl)
		{
		  char *dot = strchr(name, '.');
		  if (dot)
		    {
		      *dot++ = '\0';
		      ext = dot;
		      f = ext_type(dot);
		    }
		}
	    }
	  else
	    {
	      fprintf(stderr, "%s:%d: malformed name\tucode line\n", ofp->file, i+1);
	      exit(1);
	    }
	}
      set_glyph(ofp, i, name, ucode, fcode, f, ext, ligl, liga, ivs);
    }
}

static enum Ofp_feature
ext_type(const char *ext)
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

static void
set_glyph(Ofp *o, int i, const char *name, const char *code, const char *fcode,
	  Ofp_feature f, const char *ext, const char *ligl, const char *liga, const char *ivs)
{
#define gp(i) (&o->glyphs[i])
  gp(i)->index = i;
  gp(i)->key = un_u(ligl?ligl:name);
  if (!hash_find(o->h_sign, (uccp)gp(i)->key))
    {
      struct Ofp_sign *sp = memo_new(o->m_sign);
      sp->key = gp(i)->key;
      hash_add(o->h_sign, (uccp)gp(i)->key, sp);
    }
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

static int
unicode_value(const char *u)
{
  while (isxdigit(*u))
    ++u;
  return *u == '\0';
}


