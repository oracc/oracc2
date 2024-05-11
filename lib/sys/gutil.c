#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include <oraccsys.h>
#include <unidef.h>
#include <gutil.h>

/****************************** GRAPHEME MANIPULATION UTILITY ROUTINES **********************************/

unsigned char *
g_strip_subdig(const unsigned char *v)
{
  const unsigned char *n = v;
  if (v)
    {
      n = (uccp)strdup((ccp)v);
      unsigned char *dest = (ucp)n;
      while (*v)
	{
	  if (0xe2 == *v)
	    {
	      if (v[1] == 0x82
		  && ((v[2] >= 0x80 && v[2] <= 0x89) || v[2] == 0x93))
		v += 3;
	      else
		*dest++ = *v++;
	    }
	  else
	    *dest++ = *v++;
	}
      *dest = '\0';	
    }
  return (ucp)n;
}

unsigned char *
g_base_of_preserve_case(const unsigned char *v)
{
  return g_strip_subdig(v);
}

unsigned char *
g_base_of(const unsigned char *v)
{
  unsigned char *b = g_strip_subdig(v), *ret = NULL;
  if (b)
    {
      ret = (ucp)strdup((ccp)g_lc(b));
      free((char*)b);
    }
  return ret;
}

int
g_index_of(const unsigned char *g, const unsigned char *b)
{
  int i = 0;
  if (!b)
    b = g_base_of(g);
  g += strlen((ccp)b);
  if (g[0])
    {
      if (strlen((ccp)g) == 6)
	{
	  i = ((g[2] - 0x80)) * 10;
	  g += 3;
	}
      if (g[2] == 0x93) /* sub x is 0xE2 0x82 0x93 */
	i = 1000;
      else
	i += (g[2] - 0x80);
    }
  else
    i = 0; /* 20240511: absence of index = index 0 so empty index X sorts before X₁ */
  return i;
}

const char *
g_sub_of(int i)
{
  switch (i)
    {
    case 1:
      return "₁";
    case 2:
      return "₂";
    case 3:
      return "₃";
    case 4:
      return "₄";
    case 5:
      return "₅";
    case 6:
      return "₆";
    case 7:
      return "₇";
    case 8:
      return "₈";
    case 9:
      return "₉";
    case 0:
      return "₀";
    }
  return NULL;
}

wchar_t
g_subdig_of(wchar_t w)
{
  switch (w)
    {
    case a_acute:
    case e_acute:
    case i_acute:
    case u_acute:
    case A_acute:
    case E_acute:
    case I_acute:
    case U_acute:
      return U_s2;
    case a_grave:
    case e_grave:
    case i_grave:
    case u_grave:
    case A_grave:
    case E_grave:
    case I_grave:
    case U_grave:
      return U_s3;
    }
  return w;
}

wchar_t
g_vowel_of(wchar_t w)
{
  switch (w)
    {
    case A_acute:
    case A_grave:
      return (wchar_t)'A';
    case E_acute:
    case E_grave:
      return (wchar_t)'E';
    case I_acute:
    case I_grave:
      return (wchar_t)'I';
    case U_acute:
    case U_grave:
      return (wchar_t)'U';
    case a_acute:
    case a_grave:
      return (wchar_t)'a';
    case e_acute:
    case e_grave:
      return (wchar_t)'e';
    case i_acute:
    case i_grave:
      return (wchar_t)'i';
    case u_acute:
    case u_grave:
      return (wchar_t)'u';
    }
  return w;
}

wchar_t *
g_wlc(wchar_t *w)
{
  wchar_t *w_end = w;

  while (*w_end && *w_end != '~' && *w_end != '\\'
	 && (*w_end != '@' || iswupper(w_end[1]))
	 && (*w_end < U_s0 || *w_end > U_s9)
	 && *w_end != U_s_x
	 )
    {
      *w_end = towlower(*w_end);
      ++w_end;
    }

  return w;
}

wchar_t *
g_wuc(wchar_t *w)
{
  wchar_t *w_end = w;

  while (*w_end && *w_end != '~' && *w_end != '\\'
	 && (*w_end != '@' || iswupper(w_end[1]))
	 && (*w_end < U_s0 || *w_end > U_s9)
	 && *w_end != U_s_x
	 )
    {
      *w_end = towupper(*w_end);
      ++w_end;
    }

  return w;
}

unsigned char *
g_lc(unsigned const char *g)
{
  wchar_t *w;
  size_t len;  
  if ((w = utf2wcs(g, &len)))
    {
      w = g_wlc(w);
      return wcs2utf(w,len);
    }
  return NULL;
}

unsigned char *
g_uc(unsigned const char *g)
{
  wchar_t *w;
  size_t len;
  if ((w = utf2wcs(g, &len)))
    {
      w = g_wuc(w);
      return wcs2utf(w,len);
    }
  return NULL;
}
