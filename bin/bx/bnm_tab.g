%{
#include <string.h>
#include "bx.h"
#include "bib.h"
%}
struct bnm_tab;
%%
namemap
name
last
rest
aka
was
init
%%
static void
bnm_tab_set(const char *name, XML_StartElementHandler s, XML_EndElementHandler e)
{
    struct bnm_tab *p = bnm_tab(name, strlen(name)); p->s = s; p->e = e;
}
void
bnm_tab_init(void)
{
    bnm_tab_set("namemap", bnm_s_namemap, bnm_e_namemap);
    bnm_tab_set("name", bnm_s_name, bnm_e_name);
    bnm_tab_set("last", bnm_s_last, bnm_e_last);
    bnm_tab_set("rest", bnm_s_rest, bnm_e_rest);
    bnm_tab_set("aka", bnm_s_aka, bnm_e_aka);
    bnm_tab_set("was", bnm_s_was, bnm_e_was);
    bnm_tab_set("init", bnm_s_init, bnm_e_init);
}
