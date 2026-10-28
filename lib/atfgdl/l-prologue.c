#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>
#include <atf.h>
#include <gdl.h>
#include <gdlstate.h>
#include "ag.h"

#define AGLTYPE Mloc
#include "ag.tab.h"
#define YY_USER_ACTION ag_user_action();
extern Mloc agloc;
const char *agfile = "<stdin>";
const char *curragfile = NULL;
static int last_unmatched_line = 0;

int agflextrace = 0, last_aglineno;
static int s_next;
static int linktext, tra_text;
static char *xs;

#define atfpool(s) (char*)pool_copy((const unsigned char*)(s),atfmp->pool);
#define atfnormpool(s) xs=(char*)normalize_ws((uccp)s);aglval.text=atfpool(xs);free(xs);s=xs=NULL

static int ag_block(const char *atcmd);
static void ag_user_action(void);

#define atfsave aglval.text=atfpool(agtext)
#define atfnorm aglval.text=atfnormpool(agtext)

#define longinit longtext_init(atfmp->pool,agtext)
#define usesave curr_use_str=atfpool(agtext)

#if 1
#define r(x) x
#else
#define r(x) fprintf(stderr, "returning %d\n", x),x
#endif

/* test */

extern const char *gdl_lexnames[];
extern int gdl_lexfld[];
extern int gdl_bilingual, gdl_legacy, gdl_lexical, gdl_unicode;
static gdl_grapheme_type_p ag_gdl_grapheme_type;

extern Pool *gdlpool;
#define gdlsave(s) (char*)pool_copy((const unsigned char*)(s),gdlpool);
static int ag_gdl_grapheme_type_l(void);
static int ag_gdl_grapheme_type_s(void);
static int CIN = 0; /* 1 if in compound */ /* update: this is probably no longer necessary */
static int GIN = 0; /* 1 if in grapheme */
static int QIN = 0; /* non-zero if in a(BA) qualifier */
static int SIN = 0; /* left context is space */

#define gconst aglval.text=agtext
#define gsave  aglval.text=gdlsave(agtext)
#define gsaveg(garg)  aglval.text=gdlsave((garg))
#define gdl_yyterminate() return (gdl_term_flag = !gdl_term_flag) ? END : YY_NULL

/* Simple stack for nesting of compounds and valuquals */
#define CVQ_MAX 6
static int cvq_stack[CVQ_MAX] = { 0, 0, 0, 0, 0, 0 };
static int cvq_top = -1;
static int cvq_pop(void);
static int cvq_push(int);
static void cvq_reset(void);

#if 0
static int ag_gdl_init(void);
static int ag_line_save(const char *t);
#endif
