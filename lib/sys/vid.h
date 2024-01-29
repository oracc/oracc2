#ifndef VID_H_
#define VID_H_

enum vid_proj { vid_proj_xmd , vid_proj_xtf };

struct vid_data
{
  Hash *vidh;
  char **ids;
  char *padded_ids;
  char *vpool;
  char *file;
  int ids_alloced;
  int ids_used;
  int max_len;
  Pool *pool;
  Hash *seen;
};

extern const char *vid_get_PQ(struct vid_data *vp, int vid);
extern const char *vid_get_id(struct vid_data *vp, int vid, enum vid_proj which_proj);
extern const char *vid_map_id(struct vid_data *vp, const char *xid);
extern struct vid_data *vid_init(void);
extern struct vid_data *vid_load_data(const char *fname);
extern void vid_dump_data(struct vid_data *vp, const char *fname);
extern void vid_free(struct vid_data*vp);
extern void vid_new_id(struct vid_data *vp, const char *xid);
extern void vid_term(struct vid_data *);

#endif /*VID_H_*/
