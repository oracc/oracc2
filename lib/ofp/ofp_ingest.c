#include <oraccsys.h>
#include "ofp.h"

static enum Ofp_feature ext_type(const char *ext);
static int fnum(const char *n);
static void set_lig_pua_binding(Ofp *o, const char *name, const char *comp);
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
  Hash *seen = hash_create(1024);
  int offset = 0;

  lp = loadfile_lines3((uccp)ofp->file, &nline, &fmem);

  ofp->nglyphs = nline;
  ofp->glyphs = calloc(ofp->nglyphs, sizeof(Ofp_glyph));
  
  int i;
  for (i = 0; i < nline; ++i)
    {
      /* ignore duplicate entries; input is programmatically generated
	 so we can use simple equivalency check without
	 canonicalizing */
      if (hash_find(seen, lp[i]))
	{
	  ++offset;
	  continue;
	}
      else
	hash_add(seen, (ucp)strdup((ccp)lp[i]), "");
      
      char *name = (char*)lp[i];
      Ofp_feature f = OFPF_BASE;
      const char *ext = NULL;
      char *liga = strstr((ccp)lp[i], "<-");
      char *comp = NULL;
      if (!liga)
	{
	  char *dotliga = strstr(name, ".liga");
	  if (dotliga)
	    {
	      if ((comp = strstr(name, "->")))
		{
		  if (' ' == comp[-1])
		    comp[-1] = '\0';
		  else
		    *comp = '\0';
		  comp += 2;
		  while (isspace(*comp))
		    ++comp;
		  set_lig_pua_binding(ofp, name, comp);
		  continue;
		}
	      else if ('.' == dotliga[5])
		{
#if 0
		  char ligaliga[strlen(name)+1], *dot;
		  strcpy(ligaliga, name);
		  dot = strchr(ligaliga, '.');
		  dot = strchr(dot+1, '.');
		  *dot = '\0';
#endif		  
		}
	      else
		{
		  /* set liga as though there were a <- LIGA component */
		  liga = (char*)pool_copy((uccp)name, ofp->p);
		  char *ldot = strchr(liga, '.');
		  *ldot = '\0';
		}
	    }
	}
      else
	{
	  if (' ' == liga[-1])
	    liga[-1] = '\0';
	  else
	    *liga = '\0';
	  liga += 2;
	  while (isspace(*liga))
	    ++liga;
	}
      char *ligl = NULL;
      const char *fcode = NULL;
      char *ucode = NULL;
      const char *ivs = NULL;
      if (liga)
	{
	  hash_add(ofp->h_liga, pool_copy((uccp)name, ofp->p), &ofp->glyphs[i]);
	  char lig[strlen(liga)+1], *l;
	  strcpy(lig, liga);
	  l = strchr(lig, '_');
	  if (l)
	    *l = '\0';
	  ligl = (char*)pool_copy((uccp)lig, ofp->p);

	  /* Now name is the glyph name (may have double
	     extension, e.g., AGRIG.liga.ss01); lig is the lead
	     ligature element; liga is the ligature sequence */
	  char *dot = strrchr(name, '.');
	  if (dot && strcmp(dot, ".liga"))
	    {
	      *dot++ = '\0';
	      ext = dot;
	      f = ext_type(dot);
	      if ((dot = strchr(dot, '.')))
		*dot = '\0';
	    }

	  if ((ivs = strstr(liga, "_uE01")))
	    ivs += 2;
	}
      else
	{
	  char *tab = strchr((ccp)lp[i], '\t');
	  if (tab)
	    {
	      ucode = (char*)lp[i]+2; /* new format is 0x12345 */
	      name = tab+1;
	      *tab = '\0';
	      char *uup = ucode;
	      while (*uup)
		{
		  *uup = toupper(*uup);
		  ++uup;
		}
	    }
	  else
	    {
	      /* UCODE.FEAT e.g., u12345.ss01, etc. */
	      char *dot = strchr(name, '.');
	      if (dot)
		{
		  *dot++ = '\0';
		  ext = dot;
		  f = ext_type(dot);
		}
	      ucode = (char*)ucode_from_name(ofp, name);
	      if (!ucode || !unicode_value(ucode))
		{
		  /*name = "-";*/
		  ucode = "0000";
		}
	    }
	}
      set_glyph(ofp, i - offset, name, ucode, fcode, f, ext, ligl, liga, ivs);
    }
  ofp->nglyphs -= offset;
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

char *
liga2useq(Ofp *ofp, const char *liga)
{
  char u[strlen(liga)*2], *dest;
  const char *src;
  src = liga;
  dest = u;
  *dest++ = 'x';
  ++src; /* skip u */
  while (*src)
    {
      if ('_' == *src)
	{
	  *dest++ = '.';
	  *dest++ = 'x';
	  src += 2; /* skip _u */
	  if ('n' == src[0] && 'i' == src[1])
	    src += 2; /* skip uni as well as just u */
	}
      else
	*dest++ = *src++;
    }
  *dest = '\0';
  return (char*)pool_copy((uccp)u, ofp->p);
}

static Osl_uentry*
get_osl(Ofp *o, Ofp_glyph *gp, char **found_as)
{
  Osl_uentry *e = NULL;
  char *up = NULL;
  if (gp->liga)
    {
      gp->useq = liga2useq(o, gp->liga);
      up = strdup(gp->useq);
      if (!(e = hash_find(o->osl->h, (uccp)up)))
	{
	  char *ivs = strstr(up, ".xE01");
	  if (ivs)
	    {
	      *ivs = '\0';
	      if (!strchr(up, '.'))
		{
		  (void)memmove(up+2, up+1, strlen(up)+1);
		  up[0] = 'U'; up[1] = '+';
		}
	      if (!(e = hash_find(o->osl->h, (uccp)up)))
		e = hash_find(o->pcsl->h, (uccp)up);
	      
	    }
	}
      if (!e) /* the ligature sequence is not a sign */
	{
	  free(up);
	  char buf[strlen(gp->ligl)+2];
	  strcpy(buf+1, gp->ligl);
	  buf[0] = 'U'; buf[1] = '+';
	  up = strdup(buf);
	  if (!(e = hash_find(o->osl->h, (uccp)up)))
	    e = hash_find(o->pcsl->h, (uccp)up);
	}
    }
  else
    {
      const char *c = (gp->code ? gp->code : gp->fcode);
      char u[strlen(c)+3];
      sprintf(u, "U+%s", c);
      up = strdup(u);
      if (!(e = hash_find(o->osl->h, (uccp)u)))
	e = hash_find(o->pcsl->h, (uccp)u);
    }
  if (!e && !strstr(up, "U+0000") && !strstr(up, ".xE01") && !strstr(up, "U+E"))
    {
      /*fprintf(stderr, "autocreating osl for %s\n", up);*/
      if ('U' == *up && '+' == *up)
	e = osl_autocreate(up);
    }
  else if (e)
    /*fprintf(stderr, "lookup of %s found %s\n", up, e->u)*/;
  /* A) Don't free this because caller wants it; B) Make sure it is
     freeable; don't return a pool_copy */
  /*free(up);*/
  *found_as = up;
  return e;
}


/* name is a liga, e.g., u125BE_u12995.liga; comp is a component,
   e.g., uF2251. We need to find the glyph for each side and mutually
   connect them */
static void
set_lig_pua_binding(Ofp *o, const char *name, const char *comp)
{
  Ofp_glyph *n_glyph = hash_find(o->h_liga, (uccp)name);
  Ofp_glyph *c_glyph = hash_find(o->h_glyf, (uccp)comp);
  if (n_glyph && c_glyph)
    {
      n_glyph->gcomp = c_glyph;
      c_glyph->gliga = n_glyph;
    }
  else
    {
      if (!n_glyph && !c_glyph)
	fprintf(stderr, "ofpx: unknown name and component %s -> %s\n", name, comp);
      else if (!n_glyph)
	fprintf(stderr, "ofpx: unknown name `%s' in %s -> %s\n", name, name, comp);
      else
	fprintf(stderr, "ofpx: unknown component `%s' in %s -> %s\n", comp, name, comp);
    }
}

static void
set_glyph(Ofp *o, int i, const char *name, const char *code, const char *fcode,
	  Ofp_feature f, const char *ext, const char *ligl, const char *liga, const char *ivs)
{
#define gp(i) (&o->glyphs[i])
  gp(i)->index = i;
  gp(i)->name = name;
  hash_add(o->h_glyf, (uccp)name, gp(i));
  gp(i)->code = code;
  gp(i)->fcode = code /*fcode*/; /* can probably discontinue this */
  gp(i)->f = f;
  if (ext)
    {
      gp(i)->f_int = (f == OFPF_OIVS ? (int)strtol(ext,NULL,16) : fnum(ext));
      gp(i)->f_chr = ext;
    }
  gp(i)->ligl = ligl;
  gp(i)->liga = liga;
  gp(i)->ivs = ivs;
  /*
   * If the liga or name is known in the sign list use liga or name as
   * key.
   *
   * If not, use ligl, the lead ligature sign, or the name as the
   * key.
   *
   * this will mean that ligatures that are not defined as
   * sequences in the sign list will be registered under their lead
   * sign.
   *
   */
  if ('-' != *gp(i)->name)
    {
      char *found_as = NULL;
      Osl_uentry *ou = get_osl(o, gp(i), &found_as);
      if (ou)
	{
	  int offset = 0;
	  if ('U' == found_as[0] && '+' == found_as[1])
	    offset = 2;
	  else if ('u' == found_as[0])
	    offset = 1;
	  gp(i)->osl = ou;
	  gp(i)->key = (ccp)pool_copy((uccp)found_as+offset, o->p);
#if 0
	  if (gp(i)->liga && strlen(found_as) != strlen(gp(i)->liga))
	    {
	      fprintf(stderr, "liga %s is not a sign\n", gp(i)->liga);
	      Ofp_liga *lp = memo_new(o);
	      lp->glyph = gp(i);
	    }
#endif
	}
      else
	{
	  gp(i)->osl = NULL;
	  if (gp(i)->code && strcmp(code, "0000"))
	    gp(i)->key = gp(i)->code;
	  else
	    gp(i)->key = gp(i)->name;
	}
      free(found_as);
    }
  else
    gp(i)->key = gp(i)->name; /* "-" for key and name */
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


