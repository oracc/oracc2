#include <oraccsys.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "runexpat.h"

int verbose = 0;

Memo *m_summaries = NULL;
List *summaries = NULL;
const char *curr_letter = NULL;

struct summ
{
  const char *letter;
  const char *entry;
  int nth;
  int page;
  int zoom;
  int znth;
  int zpage;
  const char *prev;
  const char *next;
};

int nletter;
int nth;
int znth;
int zpage;

void
summary(const char *ref)
{
  struct summ *sp = memo_new(m_summaries);
  sp->entry = strdup(ref);
  sp->nth = ++nth;
  sp->page = (nth/25 + (nth%25?1:0));
  sp->zoom = nletter;
  sp->letter = curr_letter;
  sp->znth = ++znth;
  sp->zpage = (znth/25 + (znth%25?1:0));
  if (list_len(summaries))
    sp->prev = ((struct summ*)list_last(summaries))->entry;
  else
    sp->prev = "#";
  list_add(summaries, sp);
}

void
ei_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "letter"))
    {
      curr_letter = strdup(findAttr(atts, "dc:title"));
      ++nletter;
      znth = 0;
    }
  else if (!strcmp(name, "summary"))
    summary(findAttr(atts, "ref"));
}

void
ei_eH(void *userData, const char *name)
{
}

void
expat_identity(const char *fname, const char *xml_id, FILE *outfp)
{
  char const *fnlist[2];
  fnlist[0] = fname;
  fnlist[1] = NULL;
  runexpatNSuD(i_list, fnlist, ei_sH, ei_eH, NULL, outfp);
}

int
main(int argc, char **argv)
{
  options(argc, argv, "");
  m_summaries = memo_init(sizeof(struct summ), 1024);
  summaries = list_create(LIST_DOUBLE);
  runexpat_omit_rp_wrap();
  expat_identity(argv[optind], NULL, stdout);
  List_node *lp;
  for (lp = summaries->first; lp; lp = lp->next)
    {
      struct summ *sp = lp->data;
      if (lp->next)
	sp->next = ((struct summ*)lp->next->data)->entry;
      else
	sp->next = "#";
      printf("%s\t%d\t%d\t%d/%s\t%d\t%d\t%s\t%s\t\n",
	     sp->entry, sp->nth, sp->page,
	     sp->zoom, sp->letter, sp->zpage, sp->znth,
	     sp->prev, sp->next);
    }
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    default: return 1;
    }
  return 0;
}
void help() {}
