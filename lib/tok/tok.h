#ifndef TOK_H_
#define TOK_H_

typedef struct tloc
{
  const char *project;
  const char *type;
  const char *file;
  const char *text_project;  
  const char *text_id;
  const char *text_name;
  const char *line_num;
  const char *line_id;
  const char *line_label;
  const char *word_id;
  const char *word_form;
  const char **keys;
  int nkeys; /* number of keys used */
  int nkeyp; /* number of pointers allocated to keys */
} Tloc;

extern void tloc_init(int many);
extern void tloc_term(void);
extern void tloc_key(const char *k, const char *v);
extern Tloc *tloc_line(char *lp);
extern Tloc *tloc_dup(void);
extern Tloc *tloc_dup_arg(Tloc *dup);
extern Tloc *tloc_get(void);
extern void tloc_show(char c);

#endif/*TOK_H_*/
