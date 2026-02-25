#ifndef _LABEL_H
#define _LABEL_H
#include "blocktok.h"

typedef struct label
{
  char type; /* 'b' if ptr is block_token; 'n' if ptr is string */
  enum block_levels level;
  const void *ptr;
} Label;

unsigned const char*check_label(unsigned const char *lab,enum e_tu_types transtype,
				unsigned const char *xid);
void ncname_init(void);
struct label *newlabel(char type, enum block_levels level, const void *p);
void update_labels(struct node *current,enum e_tu_types transtype);
void label_segtab(const char *st,const unsigned char *tok);
void update_mlabel(enum e_type type, unsigned const char *tok);
const unsigned char *line_label(const unsigned char *tok,enum e_tu_types transtype,
				const unsigned char *xid);
void reset_labels(void);
void label_term(void);
void label_frag(struct node *current,unsigned const char *l);
const unsigned char *label_from_line_id(const unsigned char *line_id);
extern const char *label_to_id(const char *qualified_id, const char *label);

#endif /*_LABEL_H*/
