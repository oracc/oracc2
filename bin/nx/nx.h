#ifndef NX_H_
#define NX_H_

#include "nxcommon.h"
#include "ns.h"

struct nx_step;

typedef enum nx_numtok { nxt_no , nxt_ng , nxt_nw , nxt_nd , nxt_nc , nxt_nz } nx_numtok;

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
  double mev; 		/* modern equivalency value */
  const uchar *me_unit; /* modern unit */
  const uchar *me_str; 	/* modern equivalency as formatted string */
  struct nx_step *steps;/* the parsed steps */
  struct nx_step *last; /* the last parsed step so far where additional steps are added */
  struct nx_step *det;  /* the system determiner, if there is one */
  struct nx_step *com;  /* the commodity, if there is one */
} nx_number;

typedef enum nx_step_type { NX_STEP_NUM = 1, NX_STEP_TOK = 2 } nx_step_type;

typedef struct nx_step_tok
{
  const uchar *tok;
  const void *data;
  ns_inst *inst;
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
} nx_result;

extern const char *nxt_str[];

struct nxt_tab { const char *name; nx_numtok tok; };
extern struct nxt_tab *nxt (register const char *str, register size_t len);

extern void nx_data(void);
extern nx_result *nx_parse(const uchar **toks, const void **data, int ntoks);
extern void nxd_show_inst(const uchar *tok, ns_inst *ip);
extern void nxd_show_start_toks(const uchar **toks, nx_numtok *nptoks, int from, int to);
extern void nxp_numbers(nx_result *r, nx_numtok *nptoks, const uchar **toks, const void**data, int from, int to);
extern void nxr_print(nx_result *r, FILE *fp);

#endif/*NX_H_*/
