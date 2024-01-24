#include <oraccsys.h>
#include <tok.h>
#include <tis.h>

static Pool *p;

static unsigned char *
langtok(const char *lang, unsigned const char *tok)
{
  char lt[strlen(lang)+strlen((ccp)tok)+3];
  sprintf(lt, "%%%s:%s", lang, tok);
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
  static char *sigs[4] = { NULL , NULL , NULL , NULL };
  
  sigs[0] = strdup(s);
  char *dot = strchr(sigs[0], '.');
  /* SIGN.. means we are done */
  if ('.' != dot[1] || '\0' != dot[2])
    {
      strcpy(dot, ".."); /* set sigs[0] to SIGN.. */
      sigs[1] = strdup(s); /* must be just SIGN.FORM. */
      /* Is there a VALUE? */
      dot = strrchr(sigs[1], '.');
      if (dot[1]) /* yes */
	{
	  dot[1] = '\0'; /* set sigs[1] to SIGN.FORM. */
	  sigs[2] = strdup(s);
	}
      /* else sigs[1] is already SIGN.FORM. */
    }
  /* else sigs[0] is already SIGN.. */

  return sigs;
}

static char **
tis_lmsig_split(char *s)
{
  static char *sigs[2];
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
      const char **kk = loch_text(r)->keys;
      for (i = 0; kk[i]; i += 3)
	{
	  char mtok[strlen((ccp)tok)+strlen(kk[i])+strlen(kk[i+1])+strlen(kk[i+2])+4];
	  sprintf(mtok, "%s|%s=%s/%s",tok,kk[i],kk[i+2],kk[i+1]);
	  tis_add(tis, (ccp)hpool_copy((ucp)mtok,p), projwid);
	}
    }
}

static void
tis_sigs(Tisp t, Trun *r, const char **sigs)
{
  int i;
  for (i = 0; sigs[i]; ++i)
    {
      unsigned char *ltp = langtok(loch_word(r)->word_lang, (uccp)sigs[i]);
      const char *pwp = pw(loch_text(r)->text_project, loch_word(r)->word_id);
      tis_add(t, (ccp)ltp, pwp);
      tis_meta(t, r, (uccp)ltp, pwp);
    }
}

static void
tis_line(Tisp t, Trun *r, unsigned char *lp)
{
  if (loch_word(r)->word_id)
    {
      char **f = tab_split((char*)lp);
      if (f[1])
	{
	  char **sigs = NULL;
	  if ('g' == *f[0])
	    {
	      sigs = tis_gsig_split(f[1]);
	      /*loch_word(r)->word_form = hpool_copy(f[3], p);*/ /* Not currently used by tok2tis */
	      /* This is a hack: word_lang will be pc/pe/sl for signlists which is not a word_lang */
	      loch_word(r)->word_lang = (ccp)hpool_copy((uccp)gsig_parse_sl_lang(f[2]), p);
	    }
	  else
	    sigs = tis_lmsig_split(f[1]);
	  
	  tis_sigs(t, r, (const char **)sigs);

	  int i;
	  for (i = 0; sigs[i]; ++i)
	    {
	      free(sigs[i]);
	      sigs[i] = NULL;
	    }
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
	tis_line(t, r, lp);
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
