#ifndef VX_H_
#define VX_H_
#include <hash.h>
#include <tree.h>
#include <xml.h>
#include <runexpat.h>

typedef struct vxud
{
  FILE *fp;
  Tree *tp;
} Vxud;

extern Hash *xtf_a, *xtf_e;

extern Tree *vx_load(const char *fn);
extern void vx_identity(Tree *tp, FILE *fp);
extern void vx_xtf_init(void);

#endif/*VX_H_*/
