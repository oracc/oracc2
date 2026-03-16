#ifndef AX_H_
#define AX_H_

#include <atf.h>

typedef enum gtok { G_NOT, G_CELL , G_FIELD , G_WORD , G_NORMW , G_NONX } Gtok;
struct gdltok { const char *name; int tok; };

/**RUN**/
typedef struct run_context
{
  Hash *known_projects;
  struct pool *pool;
  struct proj_context *proj;
  List *langs;
} Run;

struct run_context *run_init(void);
void run_term(struct run_context *);
/**RUN**/

/**PRJ**/
struct proj_context
{
  char *name;			/* project name, e.g., saao/saa01 */
  struct xpd *xpd;
  Hash *langs;		/* languages used in project; keys are
				   langtags values are lang_context */
  int tried_qpn;	   	/* did we already try PNs for this project? */
  struct run_context *owner;
};
extern struct proj_context *curr_project;

/* Each project in a run gets an entry in here pointing to its
   proj_context */
extern Hash *known_projects;

extern void proj_init(struct run_context *runp, const char *project);
extern void proj_term(struct proj_context *);
extern void proj_default(struct run_context *r);
/**PRJ**/

extern int odt_serial, status, exit_status;

extern int harvest_notices, lem_autolem, lem_dynalem, lem_forms_raw,
  named_ents, perform_dsa, warn_unlemmatized;

/* return -1 on error; 0 for done processing; 1 for descend into child
   nodes */
typedef int (*axjoxfncp)(Node *np, void *vp);
struct axjoxfnc
{
  const char *name;
  axjoxfncp func;
  int wrapper;
};
extern struct axjoxfnc *axjoxfnc(const char *, size_t);

extern void ax_parse(Node *np, char *data);
extern void ax_gparse(Node *np, char *data);

extern struct xcl_context *ax_xcl(struct run_context *run, struct node *text);
extern void xcl_sentence_labels(struct xcl_context *xc, struct xcl_c *c);

extern void ax_lem(struct run_context *run, XCL *xcp);

extern void ax_atf(ATF*a);
extern void ax_jox(Tree*tp);

extern int ax_jox_bib(Node *np, Bib *bp);
extern int ax_jox_lg(Node *np, Group *gp);
extern int ax_jox_note(Node *np, Note *bp);
extern int ax_jox_protocol(Node *np, void *p);
extern void ax_jox_block(Node *np, Block *p);

extern int grx_jox_gdl(Node *np, Node *gdl);

#endif/*AX_H_*/
