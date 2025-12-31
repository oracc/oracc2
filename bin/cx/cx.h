#ifndef CX_H_
#define CX_H_

typedef struct cx
{
  const char *project;
  Roco *r;
} Cx;

extern void cx_roco(Cx *cp);
extern int cx_roco_id_index(Cx *c);

#endif/*CX_H_*/
