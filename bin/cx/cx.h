#ifndef CX_H_
#define CX_H_

typedef struct cx
{
  const char *project;
  Roco *r;
} Cx;

extern void cx_roco(Cx *cp);

#endif/*CX_H_*/
