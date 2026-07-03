#include <oraccsys.h>
#include <stck.h>
#include "gdl.h"
#include "gdlstream.h"

/* A stream is a composite of two numbers, the main stream and the
 * substream; they are both in the range 0..9 and are encoded as a
 * char where the main stream is *= 10.
 *
 * Hence, main stream 0/substream 0 is 10; main stream 4 substream 3
 * is 43.
 */

static Stck *sstck = NULL;
static char *stckstr[] = { "0" , "1" , "2" , "3" , "4" ,
			   "5" , "6" , "7" , "8" , "9" };

void
strm_push(char s)
{
  if (!sstck)
    sstck = stck_init(10);
  stck_push(sstck, s);
}

char
strm_pop(void)
{
  if (sstck)
    return stck_pop(sstck);
  else
    return 10; /* return default stream state if stream stack is empty */
}

void
strm_reset(void)
{
  stck_reset(sstck);
}

void
strm_term(void)
{
  stck_term(sstck);
  sstck = NULL;
}

const char *
strm_str(unsigned char s)
{
  if (s < 100)
    return stckstr[s/10];
  else
    return "0";
}

const char *
strm_sub(unsigned char s)
{
  if (s < 100)
    return stckstr[s%10];
  else
    return "0";
}
