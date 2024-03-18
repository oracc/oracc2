#ifndef VIDO_H_
#define VIDO_H_

#include <hash.h>
#include <pool.h>

typedef struct vido
{
  Hash *vidh;
  char **ids;
  char *padded_ids;
  char *vpool;
  char *file;
  int ids_alloced;
  int ids_used;
  int max_len;
  int dotstop;
  Pool *pool;
  Hash *seen;
  char prefix;
} Vido;

extern const char *vido_get_id(Vido *vp, int vid);
extern const char *vido_map_id(Vido *vp, const char *xid);
extern const char *vido_new_id(Vido *vp, const char *xid);
extern Vido *vido_init(char prefix, int dotstop);
extern Vido *vido_load_data(const char *fname, int hash_too);
extern void vido_dump_data(Vido *vp, const char *fname, const char *tsvname);
extern void vido_free(Vido*vp);
extern void vido_term(Vido *vp);

#endif /*VIDO_H_*/
