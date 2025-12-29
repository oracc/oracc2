#include <oraccsys.h>
#include <lng.h>
#include "px.h"
#include "isp/isp.h"

static const char *find_pos(const char *sig);
static char *find_lang(const char *sig, char **endp);
static const char *unweb_sig(const char *sig);
static void sigmap_ent(Isp *ip, const char *ent, const char *lng, const char *cbd);

int
sigmap_item(Isp *ip)
{
  ip->sig = unweb_sig(ip->sig);

  /* neo ignores language dialect -x-XXXXXX codes */
  if (!strcmp(ip->project, "neo"))
    {
      char *p = strchr(ip->sig, '%');
      while (':' != *p)
	if ('-' == *p)
	  break;
	else
	  ++p;
      if ('-' == *p)
	{
	  char *c = strchr(p, ':');
	  memmove(p,c,strlen(c)+1);
	}
    }

  const char *pos = find_pos(ip->sig);
  const char *lng = find_lang(ip->sig, NULL);

  /* remove any script code */
  char *hyph = strrchr(lng, '-');
  if (hyph && isdigit(hyph[1]))
    *hyph = '\0';
  
  if (pos && strlen(pos) == 2 && pos[1] == 'N')
    lng = "qpn";

  if (lng && *lng)
    {
      char cbd[strlen(ip->oracc)+strlen(ip->project)+strlen(lng)+strlen("//pub/cbd/0")];
      sprintf(cbd, "%s/pub/%s/cbd/%s", ip->oracc, ip->project, lng);
      const char *ent = dbx_one_off(cbd, "map", ip->sig, NULL);
      if (ent)
	{
	  sigmap_ent(ip, ent, lng, cbd);
	  return 0;
	}
      else
	{
	  char localsig[strlen(ip->project)+strlen(ip->sig)];
	  sprintf(localsig, "@%s", ip->project);
	  const char *pct = strchr(ip->sig, '%');
	  if (pct)
	    {
	      strcat(localsig, pct);
	      const char *ent = dbx_one_off(cbd, "map", localsig, NULL);
	      if (ent)
		{
		  sigmap_ent(ip, ent, lng, cbd);
		  return 0;
		}
	    }
	  ip->err = px_err("signmap_item found no sig in db %s:map", cbd);
	}
    }
  else
    {
      ip->err = px_err("signmap_item no lang or bad lang in sig");
    }
  return 1;
}

static void
sigmap_ent(Isp *ip, const char *ent, const char *lng, const char *cbd)
{
  ip->glos = (ccp)pool_copy((ucp)lng, ip->p);
  ip->glosdata.dir = (ccp)pool_copy((ucp)cbd, ip->p);
  ip->glosdata.web = (ccp)pool_copy((ucp)cbd, ip->p);
  char *p = strstr(ip->glosdata.web, "/pub/");
  p[1] = p[2] = p[3] = 'w';
  ip->glosdata.ent = ent;
  ip->glosdata.ipath = (ccp)pool_alloc(strlen(ip->glosdata.web)+strlen(ip->glosdata.ent)+strlen("//.html0"), ip->p);
  sprintf((char*)ip->glosdata.ipath, "%s/%s.html", ip->glosdata.web, ip->glosdata.ent);
}

static const char *
find_pos(const char *sig)
{
  static char buf[5];
  const unsigned char *p = NULL;
  const unsigned char *epos;
  p = (ucp)strstr(sig, "//");
  if (!p)
    {
      fprintf(stderr, "find_pos: no POS found in sig %s\n", sig);
      return NULL;
    }
  while (*p && ']' != *++p)
    ;
  if (!*p)
    return NULL;
  ++p;
  if ((epos = (ucp)strchr((ccp)p, '\'')))
    {
      if (epos - p < 5)
	{
	  strncpy(buf,(ccp)p,epos-p);
	  buf[epos-p] = '\0';
	}
      else
	*buf = 'X';
    }
  else
    {
      char *bufp = buf;
      while (isalpha(*p) && bufp - buf < 5)
	*bufp++ = *p++;
      *bufp = '\0';
    }
  return buf;
}

static char *
find_lang(const char *sig, char **endp)
{
  static char buf[128];
  const char *p = strchr(sig, '%');
  if (p)
    {
      const char *c = strchr(++p, ':');
      if (c - p < 128)
	{
	  if (endp)
	    *endp = (char*)c;
	  strncpy(buf, p, c-p);
	  buf[c-p] = '\0';
	  langtag_pool_init();
	  struct lang_tag *ltp = langtag_parse(buf, NULL, -1);
	  if (ltp)
	    free(ltp);
	  else
	    fprintf(stderr, "sigmap: malformed language tag (%s)\n", buf);
	  langtag_pool_term();
	}
      else
	fprintf(stderr, "sigmap: lang too long (%s)\n", p);
    }
  else
    *buf = '\0';
  return buf;
}

static const char *
unweb_sig(const char *sig)
{
  wchar_t *wsig = NULL;
  size_t nwchar = 0, i;
  char *dsig = '%' == *sig ? CGI_decode_url(sig) : sig;
  wsig = malloc((strlen(dsig)+1) * sizeof(wchar_t));
  if ((nwchar = mbstowcs(wsig, dsig, strlen(dsig)+1)) == (size_t)-1)
    fprintf(stderr, "mbstowcs failed\n");

  for (i=0; i < nwchar; ++i)
    {
      switch(wsig[i])
	{
	case 0xb1:
	  wsig[i] = L'+';
	  break;
	case 0xb4:
	  wsig[i] = L'\'';
	  break;
	case 0x20e5:
	  wsig[i] = L'\\';
	  break;
	}
    }

  char *msig = malloc(2*strlen(dsig)+1);
  wcstombs(msig, wsig, 2*strlen(dsig));
  return msig;
}
