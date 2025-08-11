#include <oraccsys.h>
#include "ncfy.h"

char innertags[128];

/* Cuneify output uses the HTML class attribute to render cuneiform
   with different fonts (fnt), script sets (set), and an appropriate
   default magnification for the font (mag). It may also use script
   (scr) information to manage splits and mergers.
 */
typedef struct class
{
  const char *fnt;
  const char *set;
  const char *mag;
  const char *scr;
} Class;

/* An element is an input item such as a grapheme, ellipsis, ZWNJ,
 * ZWJ, or word-boundary.
 *
 * The output structure consists of spans which manage the breakage
 * state. Within each span is a sequence of graphemes, ellipses, and
 * word-spaces, ZWNJ, or ZWJ.
 *
 * The input is read into an array and is only output when the next
 * XTF line is encountered or at the closing XTF tag in the case of
 * the last line.
 *
 * If a reordering line is encountered, that is used as the sequence
 * for grapheme output. Otherwise the output follows the main
 * transliteration stream, obeying any local reordering of graphemes
 * that may be present.
 *
 * Breakage state is recorded in the grapheme structures:
 * conceptually, the states are broken; damaged; or clear.
 *
 * If the font referenced by the current class provides a ligature
 * table, that is checked for immediately adjacent sequences that are
 * in the ligtable--sequences are broken by word boundaries and by
 * ZWNJ.
 *
 * If a ligtable sequence is matched, the breakage state is adjusted
 * to ensure that the ligature falls within the enclosing breakage
 * span. If a sequence mixes breakage states, all of the breakage is
 * set to damaged as an approximation of the state of the ligature
 * sequence as a whole.
 */

typedef enum elt_type { ELT_G /*grapheme*/, ELT_W /*word*/, ELT_J /*ZWJ*/, ELT_N /*ZWNJ*/ } Etype;
typedef enum brk_type { BRK_NONE /*clear*/, BRK_HASH /*damaged*/, BRK_LOST /*broken*/ } Btype;

typedef struct elt
{
  Etype etype;	/* the element type */
  const char*u8;/* the utf8 to output for the element */
  Btype btype;  /* the breakage type */
  Class *c;	/* the current class for the grapheme; usually set at
		   start of file but may be switched grapheme by
		   grapheme */
} Elt;

List *cfy_line; /* A list of pointers to Elts; initially the XTF line, but
	       reset and rebuilt from the sequence line if there is
	       one. */

Memo *m_elt;
Memo *m_class;
Hash *lig;
Pool *hp;

static Btype
breakage(const char *name, const char **atts)
{
  const char *utf8 = findAttr(atts, "g:utf8");
  const char *b = findAttr(atts, "g:break");
  if (!strcmp(b, "missing"))
    return BRK_LOST;
  else if (!strcmp(b, "damaged") || 'x' == *utf8 || 'X' == *utf8)
    return BRK_HASH;
  return BRK_NONE;
}

static void
cfy_grapheme(const char **atts, const char *utf8, Btype brk, Class *cp)
{
  if (!line)
    line = list_create(LIST_DOUBLE);
  Elt *ep = memo_new(mem_d);
  ep->etype = ELT_G;
  ep->u8 = utf8;
  ep->btype = brk;
  ep->c = cp;
  list_add(line, ep);

  /* add ZWJ/ZWNJ if the boundary requires it */
  const char *b = findAttr(atts, "g:boundary");
  if ('+' == *b)
    list_add(line, e_zwj);
  else if ('\\' == *b)
    list_add(line, e_zwnj);
}

void
cfy_reader_init(void)
{
  innertags['c'] = innertags['n'] = innertags[q] = 1;
  m_elt = memo_init(sizeof(Elt), 1024);
  m_class = memo_init(sizeof(Class), 8);
  hp = hpool_create();
}

void
cfy_reader_reset(void)
{
  memo_reset(m_elt);
}

void
cfy_reader_term(void)
{
  memo_free(m_elt);
  memo_free(m_class);
  if (lig)
    hash_free(lig);
  pool_term(p);
  pool_term(hp);
}

void
cfy_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "xcl"))
    longjmp(done, 0);

  if (!strcmp(name, "transliteration") || !strcmp(name, "composite"))
    {
      const char *xn = (ccp)xmlify((uccp)findAttr(atts, "n"));
      if (!project)
	{
	  const char *prj = findAttr(atts, "project");
	  if (*prj)
	    project = (ccp)pool_copy((uchar*)prj, p);
	}
      Cun_class *cp = cun_class(atts, curr_cp);
      if (cp->fnt)
	{
	  char ligf[strlen(oracc()) + strlen("/lib/data/ofs-.lig0") + strlen(cp->fnt)];
	  sprintf(ligf, "%s/lib/data/ofs-%s.lig", oracc(), cp->fnt);
	  lig_load(ligf);
	}
      *curr_cp = *cp;
      cfy_head(outfp, xn, cp);
    }
  else
    {
      if (in_l && !inner)
	{
	  const char *utf8 = findAttr(atts, "utf8");
	  if (*utf8)
	    cfy_grapheme(atts, utf8, breakage(atts), cun_class(atts, curr_cp));
	  else if (':' == name[1] && 'g' == name[0] && innertags[name[2]])
	    ++inner;
	  else if (!strcmp(name, "g:x"))
	    cfy_x(atts, breakage(atts), cun_class(atts, curr_cp));

	}
    }
  if (!strcmp(name, "l"))
    {
      if (cfy_line)
	cfy_line_term();
      cfy_line_init();
    }
}

void
cfy_eH(void *userData, const char *name)
{
  if (!strcmp(name, "l"))
    in_l = 0;
  else if (!strcmp(name, "g:n"))
    in_n = 0;
  else if (!strcmp(name, "g:q"))
    in_q = 0;
  else if (!strcmp(name, "g:c"))
    in_c = 0;
  else if (!strcmp(name, "g:w"))
    cfy_word_space();
}
