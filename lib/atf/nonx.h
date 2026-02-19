#ifndef _NONX_H
#define _NONX_H

/*struct node;*/ /*only for nonx_link_data which will be reimplemented*/

enum x_class { x_qual, x_extent, x_scope, x_state, x_rule , x_docket ,
	       x_impression , x_image_class , x_empty_class , x_ignore };

enum x_type { x_number, x_all, x_blank, x_broken, x_column, x_effaced, x_end, 
	      x_line, x_middle, x_object, x_rest, x_start, x_surface, x_dock ,
	      x_traces, x_illegible, x_single, x_double, x_triple, x_impress,
	      x_about, x_atleast, x_atmost , x_of, x_at , x_space, x_image,
	      x_empty  };

typedef struct nonx_token
{
  const char *name;
  enum x_class class;
  enum x_type  type;
  const char *const str;
  int of_flag;
  int pl_flag;
} Nonx_tok;

typedef struct nonx_link
{
  const char *pre;
  const char *url;
  const char *text;
  const char *post;
} Nonx_link;

typedef struct nonx
{
  int strict;
  unsigned char *number;
  unsigned char *ref;
  unsigned char *literal;
  unsigned char flags[4];
  struct nonx_token* qual;
  struct nonx_token* extent;
  struct nonx_token* scope;
  struct nonx_token* state;
  struct nonx_link * link;
} Nonx;

extern struct nonx*parse_nonx(unsigned char *l);
extern struct nonx_token *nonxtok(register const char *str,size_t len);
extern struct nonx_link *nonx_link(unsigned char *l, char *http);
/*extern void nonx_link_data(struct nonx_link *nonxp, struct node *tmp);*/

extern int dollar_fifo;
extern void dollar_init(void);
extern const char *dollar_add(const char *xid);
extern const char *dollar_get(void);
extern int dollar_clear(void);
extern void dollar_reset(void);
extern int dollar_term(void);
extern const char *dollar_peek(void);
#endif /*_NONX_H*/
