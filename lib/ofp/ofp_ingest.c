#include <oraccsys.h>
#include "ofp.h"

static void set_lig_pua_binding(Ofp *o, const char *name, const char *comp);
static void set_glyph(Ofp *o, int i, const char *name, const char *code,
		      const char *ligl, const char *liga);
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
      if (hash_find(seen, lp[i]) || '.' == *lp[i])
	{
	  ++offset;
	  continue;
	}
      else
	hash_add(seen, (ucp)strdup((ccp)lp[i]), "");
      
      char *name = (char*)lp[i];
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
	  else if ((comp = strstr(name, "->")))
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
      char *ucode = NULL;
      if (liga)
	{
	  if (ofp->trace)
	    fprintf(ofp->trace, "ofp_ingest: hash_add h_liga %s gp(%d)\n", name, i);
	  hash_add(ofp->h_liga, pool_copy((uccp)name, ofp->p), &ofp->glyphs[i]);
	  char lig[strlen(liga)+1], *l;
	  strcpy(lig, liga);
	  l = strchr(lig, '_');
	  if (l)
	    *l = '\0';
	  ligl = (char*)pool_copy((uccp)lig, ofp->p);
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
	  else if (strchr((ccp)lp[i], ' '))
	    {
	      fprintf(stderr, "%s:%d: space character in line\n", ofp->file, i);
	      continue;
	    }
	  else
	    {
	      /* UCODE.FEAT e.g., u12345.ss01, etc. */
	      char *dot = strchr(name, '.');
	      if (dot)
		*dot = '\0';
	      ucode = (char*)ucode_from_name(ofp, name);
	      *dot = '.'; /* preserve features in name */
	      if (!ucode || !unicode_value(ucode))
		{
		  /*name = "-";*/
		  ucode = "0000";
		}
	    }
	  if (hash_find(ofp->h_glyf, (uccp)name))
	    {
	      /* this indicates a name with multiple unicode values */
	      static char buf[7];
	      strcpy(buf, "u");
	      strcat(buf, ucode);
	      name = pool_copy(buf, ofp->p);
	    }
	  if (ofp->trace)
	    fprintf(ofp->trace, "ofp_ingest: hash_add h_glyf %s gp(%d)\n",
		    name, i-offset);
	  hash_add(ofp->h_glyf, pool_copy((uccp)name, ofp->p), &ofp->glyphs[i-offset]);
	}
      set_glyph(ofp, i - offset, name, ucode, ligl, liga);
    }
  ofp->nglyphs -= offset;
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
      const char *c = gp->code;
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

static int
name_is_liga(const char *name)
{
  const char *l = strstr(name,".liga");
  if (l)
    return '\0' == l[5];
  else
    return 0;
}

/* name is a liga, e.g., u125BE_u12995.liga; comp is a prebuilt
   ligature, e.g., uF2251. We need to find the glyph for each side and
   mutually connect them */
static void
set_lig_pua_binding(Ofp *o, const char *name, const char *comp)
{
  Ofp_glyph *n_glyph = hash_find(name_is_liga(name)?o->h_liga:o->h_glyf, (uccp)name);
  Ofp_glyph *c_glyph = hash_find(o->h_glyf, (uccp)comp);
  if (n_glyph && c_glyph)
    {
      /* don't assign gcom/gref in same glyphs */
      if (n_glyph->index != c_glyph->index)
	{
	  n_glyph->gcom = c_glyph->code;
	  c_glyph->gref = n_glyph->liga ? n_glyph->liga : n_glyph->name;
	}
      if (o->trace)
	fprintf(o->trace, "set_lig_pua_binding %s=>%s , %s=>%s\n",
		name, n_glyph->gcom,
		comp, c_glyph->gref);
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
set_glyph(Ofp *o, int i, const char *name, const char *code,
	  const char *ligl, const char *liga)
{
#define gp(i) (&o->glyphs[i])
  gp(i)->index = i;
  gp(i)->name = name;
  gp(i)->code = code;
  gp(i)->ligl = ligl;
  gp(i)->liga = liga;

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


