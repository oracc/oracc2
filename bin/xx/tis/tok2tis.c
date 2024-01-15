#include <oraccsys.h>
#include <tok.h>

static Pool *p;

static unsigned char *
langtok(const char *lang, unsigned char *tok)
{
  char lt[strlen(lang)+strlen((ccp)tok)+2];
  sprintf(lt, "%s%s", tok, lang);
  return hpool_copy((ucp)lt, p);
}

static const char *
pw(const char *pro, const char *wid)
{
  char PW[strlen(pro)+strlen(wid)+2];
  sprintf(PW, "%s:%s", pro, wid);
  return (ccp)hpool_copy((ucp)PW,p);
}

/** SIGN.FORM.VALUE */
static char **
tis_gsig_split(char *s)
{
  char *sigs[3] = { NULL , NULL , NULL , NULL };

  sigs[0] = strdup(s);
  
  char *dot = strchr(sigs[0], '.');
  *dot++ = '\0';
  if ('.' != *dot)
    {
      /* FORM is non-empty, can be SIGN.FORM. or SIGN.FORM.VALUE */
      sigs[1] = strdup(s);
      dot = strchr(sigs[1], '.');
      *dot++ = '\0';
      if (*dot)
	sigs[2] = strdup(s);
    }
  else
    {
      /* FORM is empty, can be SIGN.. or SIGN..VALUE */
      ++dot;
      if (*dot)
	sigs[1] = strdup(s);
    }
  return sigs;
}

static char **
tis_lmsig_split(char *s)
{
  char *sigs[2];
  sigs[0] = strdup(s);
  sigs[1] = NULL;
  return sigs;
}

static void
tis_meta(Tisp tis, Trun *r, unsigned const char *tok, const char *projwid)
{
  if (loch_text(r)->keys)
    {
      int i;
      const char *kk = loch_text(r)->keys;
      for (i = 0; kk[i]; ++i)
	{
	  char mtok[strlen((ccp)tok)+strlen(kk[i])+strlen(kk[i+1])+strlen(kk[i+2])+4);
	  sprintf(mtok, "%s|%s=%s%s",tok,kk[i],kk[i+1],kk[i+2]);
	  tis_add(tis, (ccp)hpool_copy((ucp)mtok,p), projwid);
	}
      free(kk);
    }
}

static void
tis_sigs(Tisp t, Trun *r, const char **sigs)
{
  int i;
  for (i = 0; sigs[i]; ++i)
    {
      unsigned char *ltp = langtok(loch_word(r)->word_lang, sigs[i]);
      const char *pwp = pw(loch_text(r)->text_project, loch_word(r)->word_id);
      tis_add(t, ltp, pwp);
      tis_meta(t, r, sigs[i], pwp);
    }
}

static void
tis_line(Trun *r, unsigned char *lp)
{
  if (loch_word(r)->word_id)
    {
      char **f = tab_split(lp);
      if (f[1])
	{
	  char **sigs = NULL;
	  if ('g' == *f[0])
	    sigs = tis_gsig_split(f[1]);
	  else
	    sigs = tis_lmsig_split(f[1]);
	  
	  tis_sigs(r, sigs);

	  int i;
	  for (i = 0; sigs[i]; ++i)
	    free(sigs[i]);
	}
      else
	fprintf(stderr, "tok2tis: no OID in line: %s\n", lp);
    }
  else
    {
      fprintf(stderr, "tok2tis: no W set: %s\n", lp);
      exit(1);
    }	
}

int
main(int argc, char **argv)
{
  size_t nbytes;
  unsigned char *lp;
  p = hpool_init();
  Tisp t = tis_init();
  Trun *r = tloc_tok_init();
  while ((lp = loadoneline(stdin,&nbytes)))
    {
      if (isupper(*lp))
	tloc_tok_line(r, lp);
      else
	tis_line(r, lp);
    }
  tis_dump(stdout, t, tis_sort(t));
  tloc_tok_term(r);
}


#if 0

struct tokloc
{
  const char *P;
  const char *L;
  const char *W;
};

Hash *langs = NULL;
struct tokloc tl;

static const char *
sig_lang(unsigned char *sig)
{
  unsigned char *pct = NULL;
  if (sig && (pct = (ucp)strchr((ccp)sig, '%')))
    {
      unsigned char *col = (ucp)strchr((ccp)pct, ':');
      if (col)
	*col = '\0';
      char *hlang = hash_find(langs, pct);
      if (!hlang)
	{
	  hlang = (char*)pool_copy(pct, p);
	  (void)hash_add(langs, (ucp)hlang, hlang);
	}
      return hlang;
    }
  else
    return NULL;
}

#endif
