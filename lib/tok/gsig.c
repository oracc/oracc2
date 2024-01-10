#include <oraccsys.h>
#include <gsig.h>

const char gsig_err[1024];

Gsig *
gsig_syntax(const char *err)
{
  strcpy(gsig_err, "gsig: ");
  strcat(gsig_err, err);
  return NULL;
}

void
gsig_print(FILE *fp, Gsig *gp, const char *id_sig_sep)
{
  fprintf(fp, "%s.%s.%s%s@%s%%%s:%c/%s=%s-%s-%s#%c%c%%%s:#%c%%%s:#%c%d#%c%d#%c%d",
	  gp->soid, gp->foid, gp->value,
	  id_sig_sep,
	  project, gp->asltype, gp->gdltype ? gp->gdltype : 'u',
	  gp->form, gp->sname, gp->fname, gp->value,
	  gp->role ? gp->role : 'u', gp->roletype ? gp->roletype : 'u', gp->lang,
	  gp->type ? gp->type : 'u', gp->logolang,
	  gp->position ? gp->position : 'u', gp->index,
	  gp->no_d_position ? gp->no_d_position : 'u', gp->no_d_index,
	  gp->c_position ? gp->c_position : 'u', gp->ce_index
	  );
}

Gsig *
gsig_parse(char *s, Gsig *gp, const char *id_sig_sep)
{
  if (!gp)
    gp = calloc(1, sizeof(Gsig));

  if (id_sig_sep)
    {
      gp->soid = s;
      while (*s && '.' != *s)
	++s;
      if ('.' != *s)
	return gsig_syntax("missing period after sign OID");
      *s++ = '\0';
      if ('.' != *s)
	{
	  gp->foid = s;
	  while (*s && '.' != *s)
	    ++s;
	  if ('.' != *s)
	    return gsig_syntax("missing period after form OID");
	}
      *s++ '\0';
      if (strncmp(s, id_sig_sep, strlen(id_sig_sep)))
	{
	  gp->value = s;
	  if (!(s = strstr(s, id_sig_sep)))
	    return gsig_syntax("separator '%s' not found", id_sig_sep);
	  *s = '\0';
	  s += strlen(id_sig_sep);
	}
    }
  if ('@' != *s)
    return gsig_syntax("expected @project");
  ++s;
  gp->project = s;
  char *end;
  if (!(s = strchr(s, '%')) || !(end = strchr(s, ':')))
    return gsig_syntax("expected %ASLTYPE: after project");
  gp->asltype = ++s;
  *end++ = '\0';
  s = end;
  gp->gdltype = *s++;
  if ('/' != *s)
    return gsig_syntax("expected '/' after @PROJECT%ASLTYPE:GDLTYPE");
  if (*++s)
    {
      gp->form = s;
      if (!(s = strchr(s, '=')))
	return gsig_syntax("expected '=' after FORM");
      *s++ = '\0';
      if (*s)
	{
	  gp->sname = s;
	  if (!(s = strchr(s, '-')))
	    return gsig_syntax("malformed SIGN-FORM-VALUE");
	  *s++ = '\0';
	  gp->fname = s;
	  if (!(s = strchr(s, '-')))
	    return gsig_syntax("malformed SIGN-FORM-VALUE");
	  *s++ = '\0';
	  if (!gp->value)
	    gp->value = s;
	  if (!(s = strchr(s, '#')))
	    return gsig_syntax("SIGN-FORM-VALUE should end with '#'");
	  *s++ = '\0';
	  if (!(gp->role = *s++)
	      || !(gp->roletype = *s++)
	      || '%' != *s)
	    return gsig_syntax("malformed ROLE/ROLETYPE/LANG");
	  gp->lang = *++s;
	  if (!(s = strchr(s, ':')))
	    return gsig_syntax("malformed LANG after ROLE/ROLETYPE");
	  *s++ = '\0';
	  if ('#' != *s++)
	    return gsig_syntax("missing '#' after ROLE/ROLETYPE/LANG");
	  if (!(gp->type = *s++))
	    return gsig_syntax("missing TYPE after ROLE/ROLETYPE/LANG#");
	  if ('%' != *s++ || (!(end = strchr(s, ':'))))
	    return gsig_syntax("malformed LOGOLANG");
	  gp->logolang = s;
	  *end++ = '\0';
	  s = end;
	  if ('#' != *s++)
	    return gsig_syntax("missing '#' after TYPE/LOGOLANG");
	  if (!(gp->position = *s++;))
	    return gsig_syntax("missing POSITION");
	  if (!(isdigit(*s)))
	    return gsig_syntax("malformed INDEX");
	  gp->index = atoi(s);
	  while (isdigit(*s))
	    ++s;
	  if ('#' != *s++)
	    return gsig_syntax("missing '#' after POSITION/INDEX");
	  if (!(gp->no_d_position = *s++;))
	    return gsig_syntax("missing NO_D_POSITION");
	  if (!(isdigit(*s)))
	    return gsig_syntax("malformed NO_D_INDEX");
	  gp->no_d_index = atoi(s);
	  while (isdigit(*s))
	    ++s;
	  if ('#' != *s++)
	    return gsig_syntax("missing '#' after NO_D_POSITION/NO_D_INDEX");
	  if (!(gp->c_position = *s++;))
	    return gsig_syntax("missing C_POSITION");
	  if (!(isdigit(*s)))
	    return gsig_syntax("malformed C_INDEX");
	  gp->c_index = atoi(s);
	  while (isdigit(*s))
	    ++s;
	  while (isspace(*s))
	    ++s;
	  if (*s)
	    return gsig_syntax("JUNK after otherwise well-formed GSIG");
	}
      else
	return gsig_syntax("missing SIGN-FORM-VALUE");
    }
  return gp;
}
