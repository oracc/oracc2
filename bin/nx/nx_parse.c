#include <oraccsys.h>
#include "ns.h"

/*nx_parse works on a list of tokens which must be simple graphemes or
  unit-words.  Any other input must be preprocessed to meet this
  requirements, and ancillary data for each token can be provided in
  the data array.  The nx_result preserves the data pointer for any
  grapheme or unit-word that has it.
 */
nx_result *
nx_parse(const uchar **toks, const void **data, int ntoks)
{
  const uchar **t = toks;
  if (-1 == ntoks)
    {
      const uchar *n = toks;
      ntoks = 0;
      while (*n)
	++ntoks;
    }
  nx_restok = calloc(ntoks+1, sizeof(nx_restok));
  int nr = 0;

  /* This loop iterates over the entire toks list */
  while (*t)
    {
      /* The inner loop finds the next num tok and the last num tok
	 regardless of system */
      int next = nxp_next_num(t);
      if (start >= 0)
	{
	  int last = nxp_last_num(t+next);
	  
	}
    }
  
  return r;
}
