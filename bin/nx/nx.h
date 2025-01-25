#ifndef NX_H_
#define NX_H_

#include "nxcommon.h"
#include "ns.h"

struct nx_step;

typedef struct nx_nonnum
{
  const uchar *tok;
  const void *data;
} nx_nonnum;

typedef struct nx_badnum
{
  nx_numtok type;
  const uchar *tok;
  const void *data;
} nx_badnum;

typedef struct nx_number
{
  int invalid;		/* used during parse to track whether a candidate is still valid or not */
  ns_sys *sys;		/* the result system for this parse */
  nx_num aev; 		/* ancient equivalency value */
  const uchar *ae_str;  /* aev formatted with base unit if any */
  nx_num mev; 		/* modern equivalency value as an nx frac */
  const uchar *me_unit; /* modern unit */
  const uchar *me_str; 	/* modern equivalency as formatted string */
  struct nx_step *steps;/* the parsed steps */
  struct nx_step *datas;/* a pointer to traverse 'steps' while reading data introduced by %%d */
  struct nx_step *last; /* the last parsed step so far where additional steps are added */
  struct nx_step *unit; /* for 1(diš) sila₃ this is a D number that has the unit set to the sila nx_step */
  struct nx_step *det;  /* the system determiner, if there is one */
  struct nx_step *com;  /* the commodity, if there is one */
  struct nx_step *ass;  /* the association, if there is one */
  const uchar *axis;
  char a_or_d;
} nx_number;

typedef enum nx_step_type { NX_STEP_NUM = 1, NX_STEP_TOK = 2 } nx_step_type;

typedef struct nx_step_tok
{
  const uchar *tok;
  enum nx_numtok type;
  const void *data;
  ns_inst *inst;
  nx_num aev;
} nx_step_tok;

/* This is a result step which comes from parsed input not from the
   internalized data--the latter is an ns_step

   1(ban₂) is an nx_step_tok; 1(diš) sila₃ is an nx_number

   without the following sila₃, 1(diš) is an nx_step_tok.
 */
typedef struct nx_step
{
  enum nx_step_type type; /* nx_step_tok or nx_number */
  union
  {
    nx_step_tok tok;
    nx_number *num;
  };
  struct nx_step *next;
  nx_num *aev; /* convenience pointer to the computed aev for the tok/num/num-unit combo */
} nx_step;

typedef enum nx_restok_t { NX_NO , NX_NU , NX_NA } nx_restok_t;

typedef struct nx_restok
{
  enum nx_restok_t type;
  union
  {
    nx_badnum nb;
    nx_nonnum no;
    nx_number **nu;
  };
} nx_restok;

/* nx_parse reads a token list and produces an nx_restok array; the
 * array is a union. Tokens that do not belong to a number are added as
 * nx_nonnum; tokens that belong to a number are rolled up under a
 * single nx_number.
 */
typedef struct nx_result
{
  nx_restok *r;
  int nr;
  nx_numtok *nptoks;
} nx_result;

/* Debug support:
 *
 * 	-D ARG
 *
 * ARG is a string of letters:
 *
 *	L lex
 *	Y yacc
 *	T tokens
 *	P parse
 *	V values
 *	M math
 *
 */
extern const char *nxd_arg;
#define nxd_lex() strchr(nxd_arg,'L')
#define nxd_yacc() strchr(nxd_arg,'Y')
#define nxd_tokens() strchr(nxd_arg,'T')
#define nxd_parse() strchr(nxd_arg,'P')
#define nxd_values() strchr(nxd_arg,'V')
#define nxd_math() strchr(nxd_arg,'M')

extern int nsb_altflag;
extern int opt_trace, math_trace;
extern const char *currnsfile;
extern const char *nxt_str[];

struct nxt_tab { const char *name; int tok; }; /* nx_numtok or integer sort index in ne.g */
struct nw_map { const char *name; const char *mapped; };

extern struct nxt_tab *nxt(register const char *str, register size_t len);
extern struct nw_map *nwmap(register const char *str, register size_t len);
extern struct nxt_tab *gc(register const char *str, register size_t len);
extern struct nxt_tab *na(register const char *str, register size_t len);
extern struct nxt_tab *nc(register const char *str, register size_t len);
extern struct nxt_tab *ne(register const char *str, register size_t len);
extern void ns_data(void);
extern nx_result *nx_parse(const uchar **toks, const void **data, int ntoks);
extern void nxd_show_aevs(nx_number *np);
extern void nxd_show_inst(const uchar *tok, ns_inst *ip);
extern void nxd_show_start_toks(const uchar **toks, nx_numtok *nptoks, int from, int to);
extern void nxd_show_sum(nx_num *sump);
extern void nxd_show_env(Hash *e);

extern void nxp_numbers(nx_result *r, nx_numtok *nptoks, const uchar **toks, const void**data, int from, int to);
extern void nxr_print(nx_result *r, FILE *fp, int nonewline);
extern void nxr_testdata(FILE *fp, nx_result *r, nx_numtok *nptoks, const uchar **toks, const void**data, int from, int to);
extern void nx_sys_aevs(ns_sys *sys);
extern void nx_values(nx_result *r);
extern void nx_values_np(nx_number *np);
extern const char *nx_step_mev(ns_step *sp);
extern void nx_add_frac(struct nx_num *n1, struct nx_num *n2);
extern void nx_mul_frac(struct nx_num *n1, struct nx_num *n2);
extern char *nx_modern(struct nx_num *mev, const char *meu);
extern void nx_simplify(struct nx_num *np);
extern nx_num nx_div_num(nx_num divide, nx_num by);
extern uchar *nxr_format_aev(ns_sys *ns, nx_num *nump);

extern void nx_init(void);
extern void nx_reset(void);
extern void nx_term(void);
extern void nx_input(void);
extern void nx_cli_input(char *const *argv);
extern void nx_exec(const uchar **toks, const uchar **data, int ntoks);
extern void nx_exec_lists(List *t, List *d);

extern void nxd_show_nxnu(nx_number *np);
extern int nxd_arg_ok(const char *arg);

extern int ns_jx(Hash *hsys, List *lsys);

extern nx_numtok nxp_tok_type(const uchar *t);

#endif/*NX_H_*/
