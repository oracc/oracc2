#include <stdio.h>
#include <string.h>
#include <xmlify.h>
#include <tree.h>
#include "gdl.h"
#include "gdlstate.h"

/* Set to 0 to get [# ... #] half-bracket indicators and 1 to omit them */
static int gs_brack_start = 1;

gdlstate_t gs_order_f[] = { gs_f_query, gs_f_bang, gs_f_star, gs_f_hash,
			    gs_f_uf1, gs_f_uf2, gs_f_uf3, gs_f_uf4, gs_f_plus };

gdlstate_t gs_order_o[] = { gs_damaged_o, gs_lost_o, gs_maybe_o, gs_det_o, gs_supplied_o,
			    gs_excised_o, gs_implied_o, gs_erased_o, gs_cancelled_o,
			    gs_superposed_o, gs_glolin_o, gs_glodoc_o, gs_surro_o, gs_group_o };

gdlstate_t gs_order_c[] = { gs_damaged_c, gs_lost_c, gs_maybe_c, gs_det_c, gs_supplied_c,
			    gs_excised_c, gs_implied_c, gs_erased_c, gs_cancelled_c,
			    gs_superposed_c, gs_glolin_c, gs_glodoc_c, gs_surro_c, gs_group_c };

const char *gs_status[] = { NULL , NULL , "maybe" , NULL, "supplied" ,
			    "excised" , "implied" , "erased", "cancelled" ,
			    NULL , NULL , NULL , NULL , NULL };

#define NFLAGS (sizeof(gs_order_f)/sizeof(gdlstate_t))
#define NBRACK (sizeof(gs_order_o)/sizeof(gdlstate_t))

static char flagbuf[NFLAGS/2 + (NFLAGS*3) + 1];
static char brackobuf[NBRACK*2];
static char brackcbuf[NBRACK*2];

const char *gs_str_f[NFLAGS] = { "?", "!", "*", "#", "f1", "f2", "f3", "f4", "+" };
const char *gs_str_a[NFLAGS] = { "g:queried", "g:remarked",
				 "g:collated", NULL,
				 "g:uflag1", "g:uflag2", "g:uflag3", "g:uflag4",
				 "g:plus" }; /* not sure what + flag is ? */
const char *gs_str_o[NBRACK] = { "[#", "[", "(", "{", "<", "<<", "<(", "((", "((-", "//", "{{", "{(", "<(", ")" };
const char *gs_str_c[NBRACK] = { "#]", "]", ")", "}", ">", ">>", ")>", "))", "))",  "))", "}}", ")}", ")>", ")" };

static void
gs_bracko(gdlstate_t sp)
{
  int i;
  *brackobuf = '\0';
  for (i = gs_brack_start; i < NBRACK; ++i)
    if (gs_is(sp,gs_order_o[i]))
      strcat(brackobuf, gs_str_o[i]);
}

static void
gs_brackc(gdlstate_t sp)
{
  int i;
  *brackcbuf = '\0';
  for (i = NBRACK-1; i >= gs_brack_start; --i)
    if (gs_is(sp,gs_order_c[i]))
      strcat(brackcbuf, gs_str_c[i]);
}

static void
gsraw_flags(gdlstate_t sp)
{
  int i;
  *flagbuf = '\0';
  for (i = 0; i < NFLAGS; ++i)
    if (gs_is(sp,gs_order_f[i]))
      strcat(flagbuf, gs_str_f[i]);
}

/* Simple routine to dump out gdlstate when printing raw XML, i.e.,
   the simple form that reflects the internal storage structure */
void
gdlstate_rawxml(FILE *fp, gdlstate_t sp)
{
  gs_brack_start = 0;
  gsraw_flags(sp);
  gs_bracko(sp);
  gs_brackc(sp);
  if (*flagbuf)
    fprintf(fp, " flags=\"%s\"", flagbuf);
  if (*brackobuf)
    fprintf(fp, " o=\"%s\"", xmlify((uccp)brackobuf));
  if (*brackcbuf)
    fprintf(fp, " c=\"%s\"", xmlify((uccp)brackcbuf));
}

void
gdlstate_props(Node *np, gdlstate_t sp)
{
  int i, canary = 0;

  if (!strcmp(np->rent->name, "g:n"))
    np = np->rent;
  
  for (i = 0; i < NFLAGS; ++i)
    if (gs_is(sp,gs_order_f[i]) && gs_str_a[i])
      gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, gs_str_a[i], "1");

  const char *status = "ok";
  if (gs_is(sp,gs_supplied))
    status = "supplied";
  else if (gs_is(sp, gs_excised))
    {
      /*canary = 1;*/
      status = "excised";
    }
  else if (gs_is(sp, gs_maybe))
    status = "maybe";
  else if (gs_is(sp, gs_implied))
    status = "implied";
  
  gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:status", status);
  if (canary)
    gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "canary", "g:n");

  if (gs_is(sp, gs_damaged_o))
    gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:ho", "1");

  if (gs_is(sp, gs_damaged_c))
    gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:hc", "1");

  if (gs_is(sp, gs_damaged))
    gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:break", "damaged");
  else if (gs_is(sp, gs_lost))
    gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:break", "missing");

  gs_bracko(sp);
  if (*brackobuf)
    gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:o", (ccp)pool_copy((uccp)brackobuf, gdlpool));
  
  gs_brackc(sp);
  if (*brackcbuf)
    gdl_prop_kv(np, GP_ATTRIBUTE, PG_GDL_INFO, "g:c", (ccp)pool_copy((uccp)brackcbuf, gdlpool));
  
}
