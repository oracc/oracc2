#include <oraccsys.h>
#include "gdl.h"
#include "gdl.tab.h"

int gdl_bilingual = 0;
int gdl_legacy = 0;
int gdl_lexical = 0;
int gdl_unicode = 0;
int gdl_term_flag = 0;
int g_literal_flag = 0;
int g_logoforce_flag = 0;

gdl_grapheme_type_p gdl_grapheme_type;
static int gdl_grapheme_type_l(void);
static int gdl_grapheme_type_s(void);

#define gsaveg(garg)  gdllval.text=gdlsave((garg))

/* Public function to call with (0) arg at start of text and with (1)
   arg when #atf: use legacy is encountered */
void
gdl_legacy_lexer(int legacy)
{
  if (legacy)
    {
      gdl_grapheme_type = gdl_grapheme_type_l;
      gdl_delim = gdl_delim_l;
      gdl_graph_node = gdl_graph_node_l;
    }
  else
    {
      gdl_grapheme_type = gdl_grapheme_type_s;
      gdl_delim = gdl_delim_s;
      gdl_graph_node = gdl_graph_node_s;
    }
}	

/* Version of gdl_grapheme_type for use when atf mode is strict, i.e., non-legacy */
static int
gdl_grapheme_type_s(void)
{
  char *g = gdltext;
  gsaveg(g);

  if (strlen(g) == 1
      && ('n' == *g || 'N' == *g))
      return GRAPHEME /*NUMBER*/; /*20260512: oxx treats n as <g:v>n</g:v>*/
  else if (isdigit(*g))
    {
      char *d = g+1;
      while (*d && isdigit(*d))
        ++d;
      if (*d == '(')
        return NUMBER;
      else if (!strncmp(g, "00", 2))
	return ZERO;
      else
	return BARENUM;
    }
  else if (sll_is_signlist(g))
    return LISTNUM;
  else
    return GRAPHEME;
}

/* Version of gdl_grapheme_type for use when atf: use legacy is set */
static int
gdl_grapheme_type_l(void)
{
  char *l = gdltext;
  char *g = (char*)gdl_unlegacy_str((Mloc*)&gdllloc, (uccp)l);
  if (g)
    {
      gsaveg(g);

      if (strcmp(g,l))
	gdl_legacy_prop = (uccp)l;
      if (strlen(g) == 1
	  && ('n' == *g || 'N' == *g))
	return GRAPHEME /*NUMBER*/; /*20260512: oxx treats n as <g:v>n</g:v>*/
      else if (isdigit(*g))
	{
	  char *d = g+1;
	  while (*d && isdigit(*d))
	    ++d;
	  if (*d == '(')
	    return NUMBER;
	  else if (!strncmp(g, "00", 2))
	    return ZERO;
	  else
	    return BARENUM;
	}
      else if (sll_is_signlist(g))
	return LISTNUM;
      else
	return GRAPHEME;
    }
  else
    {
      return IGNORE;
    }
}
