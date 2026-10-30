#include <string.h>
#include <ctype128.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sll.h>
#include "atf2utf.h"

#define xstrcpy(a,b) strcpy(((char *)(a)),((char*)(b)))
#define xstrlen(a)   strlen(((const char *)(a)))

const char *unitab[] =
  {
    [a_acute] = "á",
    [e_acute] = "é",
    [i_acute] = "í",
    [u_acute] = "ú",
    [A_acute] = "Á",
    [E_acute] = "É",
    [I_acute] = "Í",
    [U_acute] = "Ú",
    [a_grave] = "à",
    [e_grave] = "è",
    [i_grave] = "ì",
    [u_grave] = "ù",
    [A_grave] = "À",
    [E_grave] = "È",
    [I_grave] = "Ì",
    [U_grave] = "Ù",
    [U_cdot] = "·",
    [U_degree] = "°",
    [U_sz] = "š",
    [U_SZ] = "Š",
    [U_s0] = "₀",
    [U_s1] = "₁",
    [U_s2] = "₂",
    [U_s3] = "₃",
    [U_s4] = "₄",
    [U_s5] = "₅",
    [U_s6] = "₆",
    [U_s7] = "₇",
    [U_s8] = "₈",
    [U_s9] = "₉",
    [U_splus] = "₊",
    [U_s_x] = "ₓ",
    [U_eng] = "ŋ",
    [U_ENG] = "Ŋ",
    [U_heth] = "ḫ",
    [U_HETH] = "Ḫ",
    [U_sadhe] = "ṣ",
    [U_SADHE] = "Ṣ",
    [U_sin] = "ś",
    [U_SIN] = "Ś",
    [U_tet] = "ṭ",
    [U_TET] = "Ṭ",
    [U_aleph] = "ʾ",
    [U_ayin] = "ʿ",
    [U_amacr] = "ā",
    [U_emacr] = "ē",
    [U_imacr] = "ī",
    [U_umacr] = "ū",
    [U_Amacr] = "Ā",
    [U_Emacr] = "Ē",
    [U_Imacr] = "Ī",
    [U_Umacr] = "Ū",
    [U_acirc] = "â",
    [U_ecirc] = "ê",
    [U_icirc] = "î",
    [U_ucirc] = "û",
    [U_Acirc] = "Â",
    [U_Ecirc] = "Ê",
    [U_Icirc] = "Î",
    [U_Ucirc] = "Û",
    [U_times] = "×",
    [U_mod_r] = "ʳ",
    [U_rplus] = "⁺",
    [U_rminus] = "⁻",
    [U_ulhsq] = "⸢",
    [U_urhsq] = "⸣",
    [U_llhsq] = "⸤",
    [U_lrhsq] = "⸥"
  };

const char *unidig[128] =
  {
    ['0'] =  "₀",
    ['1'] =  "₁",
    ['2'] =  "₂",
    ['3'] =  "₃",
    ['4'] =  "₄",
    ['5'] =  "₅",
    ['6'] =  "₆",
    ['7'] =  "₇",
    ['8'] =  "₈",
    ['9'] =  "₉"
  };

enum a2u_state { a2u_none , a2u_rx_curly , a2u_rx_square , a2u_in_cg };

unsigned char *
atf2utf(Mloc *locp, register const unsigned char *s, int rx)
{
  static unsigned char buf[1024];
  register unsigned char *t = buf;
  int slflag = 0;
  enum a2u_state state = a2u_none;

  /* This could be reimplemented using sll_is_signlist if necessary */
  if (isupper(s[1]) && strlen((char*)s) > 3)
    slflag = (NULL != sll_is_signlist((ccp)s));
  else if (*s == 'N' && isdigit(s[1]))
    slflag = 1;

  *buf = '\0';
  if (slflag)
    {
      while (t-buf<1020 && (isupper(*s) || isdigit(*s)))
	*t++ = *s++;
    }

  while (*s)
    {
      if (t-buf > 1020)
	abort();
      switch (*s)
	{
	case '{':
	  if (rx)
	    state = a2u_rx_curly;
	  *t++ = *s++;
	  break;
	case '}':
	  if (rx)
	    state = a2u_none;
	  *t++ = *s++;
	  break;
	case '[':
	  if (rx)
	    state = a2u_rx_square;
	  *t++ = *s++;
	  break;
	case ']':
	  if (rx)
	    state = a2u_none;
	  *t++ = *s++;
	  break;
	case '\\':
	  if (s[1] == '&')
	    {
	      ++s;
	    }
	  else
	    {
	      *t++ = '\\';
	      ++s;
	      if (*s)
		*t++ = *s++;
	    }
	  break;
	case '|':
	  if (state == a2u_in_cg)
	    state = a2u_none;
	  else
	    state = a2u_in_cg;
	  *t++ = *s++;
	  break;
	case '.':
	case '+':
	case '&':
	case '%':
	case '@':
	case ':':
	case '-':
	  *t++ = *s++;
	  if (isupper(s[1])
	      && ((strlen((char*)s)>3 && sll_is_signlist((ccp)s))
		  || ('N' == *s && isdigit(s[1])))
	      )
	    {
	      while (t-buf<1020 && (isupper(*s) || isdigit(*s)))
		*t++ = *s++;
	    }
	  break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  if (!rx || state != a2u_rx_curly)
	    {
	      xstrcpy(t,unidig[*s]);
	      t+=xstrlen(t);
	      ++s;
	      if (isdigit(*s))
		{
		  xstrcpy(t,unidig[*s]);
		  t+=xstrlen(t);
		  ++s;
		}
	    }
	  else
	    *t++ = *s++;
	  break;
	case 's':
	  switch (s[1])
	    {
	    case ',':
	      /*case 'y':*/
	      xstrcpy(t,unitab[U_sadhe]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case '\'':
	      xstrcpy(t,unitab[U_sin]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'z':
	      xstrcpy(t,unitab[U_sz]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    default:
	      *t++ = *s++;
	    }
	  break;
	case 'S':
	  switch (s[1])
	    {
	    case ',':
	      /*case 'y':*/
	      xstrcpy(t,unitab[U_SADHE]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case '\'':
	      xstrcpy(t,unitab[U_SIN]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'Z':
	      xstrcpy(t,unitab[U_SZ]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    default:
	      *t++ = *s++;
	    }
	  break;
	case 'x':
	  if (state == a2u_in_cg)
	    {
	      xstrcpy(t,unitab[U_times]);
	      t+=xstrlen(t);
	      ++s;
	    }
	  else
	    {
	      if (*buf && isalpha(s[-1]) 
		  && (!s[1] || s[1] == '-' || isspace(s[1]) || s[1] == '.'))
		{
		  xstrcpy(t,unitab[U_s_x]);
		  t+=xstrlen(t);
		  ++s;
		}
	      else
		*t++ = *s++;
	    }
	  break;
	case 'X':
	  if (*buf && isalpha(s[-1]))
	    {
	      xstrcpy(t,unitab[U_s_x]);
	      t+=xstrlen(t);
	      ++s;
	    }
	  else
	    *t++ = *s++;
	  break;
	case 'j':
	  xstrcpy(t,unitab[U_eng]);
	  t+=xstrlen(t);
	  ++s;
	  break;
	case 'J':
	  xstrcpy(t,unitab[U_ENG]);
	  t+=xstrlen(t);
	  ++s;
	  break;
	case 't':
	  if (s[1] == ','/* || s[1] == 'y'*/)
	    {
	      xstrcpy(t,unitab[U_tet]);
	      t+=xstrlen(t);
	      s+=2;
	    }
	  else
	    *t++ = *s++;
	  break;
	case 'T':
	  if (s[1] == ','/* || s[1] == 'y'*/)
	    {
	      xstrcpy(t,unitab[U_TET]);
	      t+=xstrlen(t);
	      s+=2;
	    }
	  else
	    *t++ = *s++;
	  break;
	case '\'':
	  xstrcpy(t,unitab[U_aleph]);
	  t+=xstrlen(t);
	  ++s;
	  break;
	case '`':
	  xstrcpy(t,unitab[U_ayin]);
	  t+=xstrlen(t);
	  ++s;
	  break;
	case 'h':
	  if (s[1] == ',')
	    {
	      xstrcpy(t,unitab[U_heth]);
	      t+=xstrlen(t);
	      s+=2;
	    }
	  else
	    *t++ = *s++;
	  break;
	case 'H':
	  if (s[1] == ',')
	    {
	      xstrcpy(t,unitab[U_HETH]);
	      t+=xstrlen(t);
	      s+=2;
	    }
	  else
	    *t++ = *s++;
	  break;
	case '^':
	  switch (s[1])
	    {
	    case 'a':
	      xstrcpy(t,unitab[U_acirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'e':
	      xstrcpy(t,unitab[U_ecirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'i':
	      xstrcpy(t,unitab[U_icirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'u':
	      xstrcpy(t,unitab[U_ucirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'A':
	      xstrcpy(t,unitab[U_Acirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'E':
	      xstrcpy(t,unitab[U_Ecirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'I':
	      xstrcpy(t,unitab[U_Icirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'U':
	      xstrcpy(t,unitab[U_Ucirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    default:
	      if (locp)
		mesg_verr(locp, "%s: unknown accent sequence",s);
	      ++s;
	      break;
	    }
	  break;
	case '=':
	  switch (s[1])
	    {
	    case 'a':
	      xstrcpy(t,unitab[U_amacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'e':
	      xstrcpy(t,unitab[U_emacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'i':
	      xstrcpy(t,unitab[U_imacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'u':
	      xstrcpy(t,unitab[U_umacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'A':
	      xstrcpy(t,unitab[U_Amacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'E':
	      xstrcpy(t,unitab[U_Emacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'I':
	      xstrcpy(t,unitab[U_Imacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'U':
	      xstrcpy(t,unitab[U_Umacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    default:
	      if (locp)
		mesg_verr(locp, "%s: unknown accent sequence",s);
	      ++s;
	      break;
	    }
	  break;
	default:
	  *t++ = *s++;
	  break;
	}
    }
  *t = '\0';
  return buf;
}
