#include <oraccsys.h>
#include "atf.h"

/*FIXME: should be dynamic*/
#define MAXLEM 1024
static void
lem_handler(struct node *parent, enum t_scope scope, 
            enum block_levels level, unsigned char *l)
{
  unsigned char *lem[MAXLEM];
  struct node *wline;
  int nlem,j;

  xcl_set_sparse_skipping(NULL);
  wline = lastChild(parent);
  if (wline && !xstrcmp(wline->names->pname,"lg"))
    {
      int i;
      for (i = 0; i < wline->children.lastused; ++i)
        if (!xstrcmp(getAttr(wline->children.nodes[i],"type"),"nts"))
          break;
      if (i < wline->children.lastused)
        wline = wline->children.nodes[i];
      else
        wline = wline->children.nodes[0];
   }
  if (!wline)
    {
      warning("no words to host lemmata");
      return;
    }
  for (nlem = 0; *l && nlem < MAXLEM; ++nlem)
    {
      lem[nlem] = l;
      while (*l && (*l != ';' || ('+' == l[-1] || '-' == l[-1] 
                                  || (l[1] && !isspace(l[1])))))
        ++l;
      if (';' == *l)
        {
          *l++ = '\0';
          while (isspace(*l))
            ++l;
          if ('\0' == *l)
            warning("spurious ';' at end of #lem: line");
          else if (';' == *l)
            warning("empty lemmatization in #lem: line");
        }
    }
  lem[nlem] = NULL;
  j = lemwords(0,wline,lem);
  if (j != nlem)
    {
      if (j < nlem)
        warning("too many lemmata");
      else
        warning("too few lemmata");
    }
}

static int
lemwords(int j, struct node *parent, unsigned char**lem)
{
  register int i;
  for (i = 0; i < nline_words; ++i)
    {
      struct node *cp = line_words[i];
      struct node *parent = cp->parent;
      if (sparse_lem && *parent->type == 'e' && !xstrcmp(parent->names->pname,"f"))
        xcl_set_sparse_skipping(((struct attr*)parent->attr.nodes[0])->valpair[1]);
      if (!sparse_skipping)
        {
          if (lem[j])
            lem_save_lemma(cp,(const char *)lem[j++]);
          else
            return j+1;
        }
    }
  return j;
}

void
lem_save_lemma(struct node *wp, const char *lemma)
{
  struct xcl_ilem /*ilem_form*/ *form = NULL;
  unsigned const char *xmlid = NULL;
  while (isspace(*lemma))
    ++lemma;
  if (lemma && strlen(lemma))
    {
      struct node *wp2 = NULL;
      if (strcmp(wp->names->pname,"g:w")
	  && strcmp(wp->names->pname,"n:w"))
	wp2 = find_w_node(wp);
      if (wp2)
	wp = wp2;
      xmlid = getAttr(wp,"xml:id");
      form = hash_find(word_form_index, xmlid);
      if (form)
	form->i->literal = (char*)npool_copy((unsigned char *)lemma,lemline_xcp->pool);
      else
	vwarning("internal error: word_form_index lookup failed; lemma=%s; xml:id=%s", lemma, xmlid);
    }
  else
    warning("empty lemmatization entry");
}
