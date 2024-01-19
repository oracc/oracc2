#include <oraccsys.h>
#include <tok.h>

Trun *
tloc_tok_init(void)
{
  Trun *r = trun_init(0);
  /* project/type are set from the .tok file */
  tlb_init(r, NULL, NULL);
  return r;
}

void
tloc_tok_term(Trun *r)
{
  tlb_term(r);
}

void
tloc_tok_line(Trun *r, unsigned char *lp)
{
  char **f = tab_split((char*)lp);
  switch (*lp)
    {
    case 'P':
      tlb_P(r, f[1]);
      break;
    case 'Y':
      tlb_Y(r, f[1]);
      break;
    case 'F':
      tlb_F(r, f[1]);
      break;
    case 'T':
      tlb_T(r, f[1], f[2], f[3]);
      break;
    case 'K':
      {
	char *v, *c;
	c = f[2];
	v = strchr(c, '/');
	*v++ = '\0';
	tlb_K(r, f[1], v, c);
      }
      break;
    case 'L':
      if (-1 == loch_text(r)->nkeys)
	{
	  tlb_K_wrapup(r);
	  if (!loch_text(r)->keys)
	    loch_text(r)->nkeys = 0;
	}
      tlb_L(r, atoi(f[1]), f[2], f[3]);
      break;
    case 'W':
      tlb_W(r, f[1], NULL, NULL); /* lang and form are not output on W so reading from .tok can't supply them */
      break;
    case 'M':
      tlb_M(r, f[1], f[2]);
      break;
    default:
      fprintf(stderr, "tloc_tok: unhandled tok line '%s'\n", f[0]);
      break;
    }
}
