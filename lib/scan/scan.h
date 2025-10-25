#ifndef _SCAN_H
#define _SCAN_H
#include <stdlib.h>
#include <tree.h>
#include <pool.h>

extern Pool *scan_pool;

typedef enum sterm { SCAN_WHITE , SCAN_LINE , SCAN_PARA , SCAN_END , SCAN_TERM_TOP } Sterm;

typedef enum stype { SCAN_SPAN , SCAN_TEXT , SCAN_TYPE_TOP } Stype;

typedef struct scan
{
  Node n;
  Stype t;
  Sterm term;
  const char *attr;
} Scan;

typedef int (*scan_block_tester)(const unsigned char *p);
unsigned char *skip_white(unsigned char *fext, size_t *lnump);
unsigned char *scan_token(unsigned char *tokp, unsigned char **endtokp, 
			  unsigned char *savep);
char* scan_curly(char *p, char **endp);
void scan_square(Scan *s, unsigned char *endtok, 
		 unsigned char **text_start, size_t *local_lnump);
void scan_text(Scan *s, unsigned char *text_start, 
	       unsigned char **text_end, size_t *local_lnump,
	       scan_block_tester f);
int block_peek(unsigned char *p, scan_block_tester f);
#endif/*_SCAN_H*/
