#include "iterator.h"

static void o_jsn_aliases(struct entry *e);
static void o_jsn_allow(struct entry *e);
static void o_jsn_bases(struct entry *e);
static void o_jsn_cbd(struct cbd *c);
static void o_jsn_dcfs(struct entry *c);
static void o_jsn_end_cbd(struct cbd *c);
static void o_jsn_end_entry(struct entry *e);
static void o_jsn_entry(struct entry *e);
static void o_jsn_forms(struct entry *e);
static void o_jsn_meta(struct entry *e);
static void o_jsn_parts(struct entry *e);
static void o_jsn_phon(struct entry *e);
static void o_jsn_root(struct entry *e);
static void o_jsn_senses(struct entry *e);
static void o_jsn_stems(struct entry *e);

static struct ifnc_map imap[] = {
  { if_alias , (iterator_fnc)o_jsn_aliases },
  { if_allow , (iterator_fnc)o_jsn_allow },
  { if_bases , (iterator_fnc)o_jsn_bases },
  { if_cbd , (iterator_fnc)o_jsn_cbd },
  { if_dcfs , (iterator_fnc)o_jsn_dcfs },
  { if_end_cbd , (iterator_fnc)o_jsn_end_cbd },
  { if_end_entry , (iterator_fnc)o_jsn_end_entry },
  { if_entry , (iterator_fnc)o_jsn_entry },
  { if_forms , (iterator_fnc)o_jsn_forms },
  { if_meta , (iterator_fnc)o_jsn_meta },
  { if_parts , (iterator_fnc)o_jsn_parts },
  { if_phon , (iterator_fnc)o_jsn_phon },
  { if_root , (iterator_fnc)o_jsn_root },
  { if_senses , (iterator_fnc)o_jsn_senses },
  { if_stems , (iterator_fnc)o_jsn_stems },
  { if_top , NULL },
};

static iterator_fnc *fncs;

static iterator_fnc *
ifnc_init(void)
{
  int i;
  fncs = calloc(if_top, sizeof(iterator_fnc));
  for (i = 0; i < if_top; ++i)
    fncs[imap[i].ix] = imap[i].fnc;
  return fncs;
}
