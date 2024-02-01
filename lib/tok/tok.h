#ifndef TOK_H_
#define TOK_H_

#include <trun.h>
#include <loch.h>
#include <gsig.h>
#include <cbdex.h>

#include "all.proto"

#undef pp
#define pp(p) ((p)?(p):"")

struct tokflags
{
  const char *name;
  const char *attr;
};

#endif/*TOK_H_*/
