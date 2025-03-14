#include <oraccsys.h>
#include <gsig.h>

/** Routines to parse gsig data.  This consists of an three-part OID
    and the signature proper, SIG.  OID and SIG can be separated by
    any string. If the OID and SIG are tab-separated and the line has
    been tab-split into fields then gsig_parse_oid and gsig_parse can
    be called separately. gsig_parse_alloc should be called before
    parsing each OID/SIG; gsig_parse_clone can be used to create a
    clean copy of the gsig.
 */

static char gsig_err[1024];

/* untested because it turned out not to be needed where I thought I needed it */
char *
gsig_index(const char *gs)
{
  if (gs)
    {
      const char *hash = gs;
      int nhash = 0;
      while (*hash && nhash < 3)
	{
	  if ('#' == *hash)
	    ++nhash;
	  ++hash;
	}
      while (*hash && !isdigit(*hash))
	++hash;
      if (*hash)
	{
	  static char index[8]; /* word with more than 7-digit grapheme count would be absurd */
	  char *i = index;
	  do
	    *i++ = *hash++;
	  while (isdigit(*hash));
	  *i = '\0';
	  return index;
	}
    }
  return NULL;
}

char *
gsig_parse_last_error(void)
{
  return gsig_err;
}

Gsig *
gsig_parse_alloc(Gsig *gp)
{
  if (!gp)
    gp = calloc(1, sizeof(Gsig));
  else
    {
      if (gp->coresig)
	free((char*)gp->coresig);
      memset(gp, '\0', sizeof(Gsig));
    }
  return gp;
}

Gsig *
gsig_parse_clone(Gsig *gp)
{
  Gsig *ngp = gsig_parse_alloc(NULL);
  *ngp = *gp;
  return ngp;
}

static Gsig *
gsig_parse_error(const char *err)
{
  strcpy(gsig_err, "gsig: ");
  strcat(gsig_err, err);
  return NULL;
}

Gsig *
gsig_parse_oid(char *s, Gsig *gp)
{
  gp->soid = s;
  while (*s && '.' != *s)
    ++s;
  if ('.' != *s)
    return gsig_parse_error("missing period after sign OID");
  *s++ = '\0';
  if ('.' != *s)
    {
      gp->foid = s;
      while (*s && '.' != *s)
	++s;
      if ('.' != *s)
	return gsig_parse_error("missing period after form OID");
    }
  *s++ = '\0';
  gp->value = s;
  return gp;
}

/* Cheaply extract the word lang from a gsig without parsing the whole thing */
const char *
gsig_parse_sl_lang(const char *s)
{
  static char lang[3]; /* this is just sl/pc/pe */
  const char *pct;
  if (s && (pct = strchr(s, '%')))
    {
      const char *colon;
      ++pct;
      if ((colon = strchr(pct, ':')))
	{
	  if (colon-pct < 3) /* should error */
	    {
	      strncpy(lang, pct, colon-pct);
	      return lang;
	    }
	}
    }
  return NULL;
}

/* Cheaply extract the word lang from a gsig without parsing the whole thing */
const char *
gsig_parse_word_lang(const char *s)
{
  static char lang[18]; /* max length is akk-x-stdbab-949, i.e., 18 */
  const char *pct;
  if (s && (pct = strchr(s, '%')))
    {
      ++pct;
      if ((pct = strchr(pct, '%')))
	{
	  const char *colon;
	  ++pct;
	  if ((colon = strchr(pct, ':')))
	    {
	      if (colon-pct < 18) /* should error */
		{
		  strncpy(lang, pct, colon-pct);
		  return lang;
		}
	    }
	}
    }
  return NULL;
}

/* Destructively parse the grapheme sig */
Gsig *
gsig_parse(char *s, Gsig *gp, const char *id_sig_sep)
{
  if (id_sig_sep)
    {
      char *g = strstr(s, id_sig_sep);
      if (g)
	{
	  *g = '\0';
	  gsig_parse_oid(g, gp);
	  s = g;
	  s += strlen(id_sig_sep);
	}
      else
	{
	  return gsig_parse_error("id/sig separator not found");
	}
    }

  if ('@' != *s)
    return gsig_parse_error("expected @project");
  ++s;
  gp->project = s;
  char *end;
  if (!(s = strchr(s, '%')) || !(end = strchr(s, ':')))
    return gsig_parse_error("expected %ASLTYPE: after project");
  gp->asltype = ++s;
  *end++ = '\0';
  s = end;
  gp->gdltype = *s++;
  if ('/' != *s)
    return gsig_parse_error("expected '/' after @PROJECT%ASLTYPE:GDLTYPE");
  if (*++s)
    {
      gp->form = s;
      if (!(s = strchr(s, '=')))
	return gsig_parse_error("expected '=' after FORM");
      *s++ = '\0';
      if (*s)
	{
	  char *t;
	  t = gp->sname = s;
	  if (!(t = strchr(s, '#')))
	    return gsig_parse_error("SIGN-FORM-VALUE should end with '#'");
	  *t++ = '\0';
	  gp->coresig = strdup(s);
	  if (!(s = strchr(s, '-')))
	    return gsig_parse_error("malformed SIGN-FORM-VALUE");
	  *s++ = '\0';
	  gp->fname = s;
	  if (!(s = strchr(s, '-')))
	    return gsig_parse_error("malformed SIGN-FORM-VALUE");
	  *s++ = '\0';
	  if (!gp->value)
	    gp->value = s;
	  s = t;
	  if (!(gp->role = *s++)
	      || !(gp->roletype = *s++)
	      || '%' != *s)
	    return gsig_parse_error("malformed ROLE/ROLETYPE/LANG");
	  gp->lang = ++s;
	  if (!(s = strchr(s, ':')))
	    return gsig_parse_error("malformed LANG after ROLE/ROLETYPE");
	  *s++ = '\0';
	  if ('#' != *s++)
	    return gsig_parse_error("missing '#' after ROLE/ROLETYPE/LANG");
	  if (!(gp->type = *s++))
	    return gsig_parse_error("missing TYPE after ROLE/ROLETYPE/LANG#");
	  if ('%' != *s++ || (!(end = strchr(s, ':'))))
	    return gsig_parse_error("malformed LOGOLANG");
	  gp->logolang = s;
	  *end++ = '\0';
	  s = end;
	  if ('#' != *s++)
	    return gsig_parse_error("missing '#' after TYPE/LOGOLANG");
	  if (!(gp->position = *s++))
	    return gsig_parse_error("missing POSITION");
	  if (!(isdigit(*s)))
	    return gsig_parse_error("malformed INDEX");
	  gp->index = atoi(s);
	  while (isdigit(*s))
	    ++s;
	  if ('#' != *s++)
	    return gsig_parse_error("missing '#' after POSITION/INDEX");
	  if (!(gp->no_d_position = *s++))
	    return gsig_parse_error("missing NO_D_POSITION");
	  if (!(isdigit(*s)))
	    return gsig_parse_error("malformed NO_D_INDEX");
	  gp->no_d_index = atoi(s);
	  while (isdigit(*s))
	    ++s;
	  if ('#' != *s++)
	    return gsig_parse_error("missing '#' after NO_D_POSITION/NO_D_INDEX");
	  if (!(gp->c_position = *s++))
	    return gsig_parse_error("missing C_POSITION");
	  if (!(isdigit(*s)))
	    return gsig_parse_error("malformed C_INDEX");
	  gp->c_index = atoi(s);
	  while (isdigit(*s))
	    ++s;
	  if ('#' != *s++)
	    return gsig_parse_error("missing '#' after C_POSITION/C_INDEX");
	  gp->preserved = *s++;
	  gp->editorial = *s++;
	  gp->flags = s;
	  while (*s && !isspace(*s))
	    ++s;
	  while (isspace(*s))
	    ++s;
	  if (*s)
	    return gsig_parse_error("JUNK after otherwise well-formed GSIG");
	}
      else
	return gsig_parse_error("missing SIGN-FORM-VALUE");
    }
  return gp;
}

/* Fish the no-determinative position code out of the gsig
 *
 * By design this routine ignores graphemes in determinatives.
 *
 */
const char *
gsig_no_d_pos(const char *gsig)
{
  static char pos[2] = { '\0', '\0' };
  const char *p = gsig + strlen(gsig);
  int i = 0;
  while (p > gsig)
    {
      if ('#' == p[-1])
	if (++i == 3)
	  break;
      --p;
    }
  if ('0' != p[1])
    {
      pos[0] = p[0];
      return pos;
    }
  else
    return NULL; /* The grapheme is ignored because it's a determinative */
}
