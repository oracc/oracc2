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

typedef enum nx_numtok { nxt_no , nxt_ng , nxt_nw , nxt_nd , nxt_nz } nx_numtok;

typedef struct nx_number
{
  ns_sys *sys;		/* the result system for this parse */
  nx_num aev; 		/* ancient equivalency value */
  double mev; 		/* modern equivalency value */
  const uchar *me_unit; /* modern unit */
  const uchar *me_str; 	/* modern equivalency as formatted string */
  struct nx_step *steps;/* the parsed steps */
  struct nx_step *last;	/* the last parsed step to which subsequent steps are added */
  struct nx_number *next; /* ambiguity is handled by making a list of possible nx_number results */
} nx_number;

typedef struct nx_step_tok
{
  const uchar *tok;
  const void *data;
  ns_step *step;
} nx_step_tok;

/* This is a result step which comes from parsed input not from the
   internalized data--the latter is an ns_step

   1(ban₂) is an nx_step_tok; 1(diš) sila₃ is an nx_number

   without the following sila₃, 1(diš) is an nx_step_tok.
 */
typedef struct nx_step
{
  int type; /* nx_step_tok or nx_number */
  union
  {
    nx_step_tok *tok;
    nx_number *num;
  };
  struct nx_step *next;
} nx_step;

typedef enum nx_restok_t { NX_NO , NX_NU } nx_restok_t;

typedef struct nx_restok
{
  int type;
  union
  {
    nx_nonnum no;
    nx_number *nu;
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

struct nxt_tab { const char *name; nx_numtok tok; };
extern struct nxt_tab *nxt (register const char *str, register size_t len);

#endif/*NX_H_*/
