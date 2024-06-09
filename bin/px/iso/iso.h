#ifndef ISO_H_
#define ISO_H_

/* Top-level structure for the outline */
struct iso
{
  unsigned char **zlines;
  size_t zmax;
  int zlev;
  List *l; /* data items are isph w level = 1 */
};

/* Zoom line split up into headers and items */
struct isoz
{
  unsigned char *h1;
  unsigned char *h2;
  unsigned char *h3;
  unsigned char *items;
  int count;
};

#include "../isp/isp.h"

#include "all.proto"

#endif/*ISO_H_*/
