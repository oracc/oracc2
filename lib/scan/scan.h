#ifndef _SCAN_H
#define _SCAN_H
#include <stdlib.h>
#include <tree.h>
#include <pool.h>

/* The scan library supports o2's inline mode, inl. There are the following
   node types:

   span	= a command that takes an argument in curly brackets
   pair = matching @["?] and ["?]@ for ATF translation  mode
   wild = any @-sequence in ATF translation mode because '@me' means 'italicize "me"'
   self	= an @name that is valid in a specific vocabulary and is passed through as itself
   escp = an escape sequence for '@', i.e., @@
   text	= a text string between or within spans
   #	= a command that takes no argument, e.g., @br, @-, @~

   The library XML writer uses the xmlns
   i=http://oracc.org/ns/inl/1.0, i.e., the output tags are

   <i:span>
   <i:text>
   <i:newline> etc.

   For spans, the tag name is given in an attribute, tag, and anything
   between square brackets is given in a second attribute, att:

   @up[2cm]{d}   =>   <i:span tag="up" att="2cm"><i:text>d</i:text></i:span>

 */

extern Pool *scan_pool;

typedef enum sterm { SCAN_WHITE , SCAN_LINE , SCAN_PARA , SCAN_END , SCAN_TERM_TOP } Sterm;

typedef enum stype { SCAN_SPAN , SCAN_TEXT , SCAN_SELF , SCAN_TYPE_TOP } Stype;


/* This is the type for the context of a call from inl, the inline
   library function that controls the scan process */
typedef struct scan
{
  const char *data;
  Tree *tree;
  Mloc mp;
} Scan;

/* This is the type for individual scanned segments */
typedef struct scanseg
{
  Scan *sp;
  Node *np;
  Stype t;
  Sterm term;
  const char *name; /* this is where the result of scan_name is put */
  const char *attr; /* this is the result of scan_quare */
} Scanseg;

extern void scan_init(void);
extern void scan_pool(Pool *p);
extern void scan_term(void);

typedef int (*scan_block_tester)(const unsigned char *p);

extern unsigned char *skip_white(Scan *sp, unsigned char *fext);
extern unsigned char *scan_token(Scan *sp, unsigned char *tokp, unsigned char **endtokp, 
				 unsigned char *savep);
extern char* scan_curly(Scan *sp, char *p, char **endp);
extern char *scan_name(Scan *sp, char *s, char **endp);
extern void scan_square(Scanseg *ssp, unsigned char *endtok, 
			unsigned char **text_start);
extern void scan_text(Scanseg *ssp, unsigned char *text_start, 
		      unsigned char **text_end, scan_block_tester f);
extern int block_peek(Scan *sp, unsigned char *p, scan_block_tester f);
extern char *scan_str_term(Scan *sp, const char *start, const char *match, char **endp);

#endif/*_SCAN_H*/
