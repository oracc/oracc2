#ifndef AX_H_
#define AX_H_

#include <atf.h>

/* return -1 on error; 0 for done processing; 1 for descend into child
   nodes */
typedef int (*axjoxfncp)(Node *np, void *vp);
struct axjoxfnc
{
  const char *name;
  axjoxfncp func;
};
extern struct axjoxfnc *axjoxfnc(const char *, size_t);

extern void ax_parse(Node *np, char *data);
extern void ax_gparse(Node *np, char *data);
extern void ax_atf(ATF*a);
extern void ax_jox(Tree*tp);

extern int ax_jox_bib(Node *np, Bib *bp);
extern int ax_jox_note(Node *np, Note *bp);
extern int ax_jox_protocol(Node *np, void *p);
extern void ax_jox_block(Node *np, Block *p);

extern int grx_jox(Node *np, const char *tag);

#endif/*AX_H_*/
