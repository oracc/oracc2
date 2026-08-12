#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"

static void
etcsl_comment(Mloc l, char *text)
{
  char *sp = strchr(text, ' ');
  if (!sp)
    sp = strchr(text, '\t');
  if (!sp)
    {
      mesg_verr(&l, "syntax error in #etcsl# comment--expected whitespace after type");
      return;
    }
  int len = sp-text;
  char type[len+1];
  strncpy(type, text, len);
  type[len] = '\0';
  struct map *mp = etcslnote(type, len);
  if (!mp)
    {
      mesg_verr(&l, "syntax error in #etcsl# unknown type %s", type);
      return;
    }
  text += len;
  while (isspace(*text))
    ++text;
  Node *np;
  if (atfmp->llines)
    {
      np = atf_node("nonx", &l);
      line_register(l, LT_DOLLAR, np, (ucp)text);
    }
  else
    np = atf_add("nonx", &l);
  sprintf(line_id_insertp,"%d", ++line_id);
  const char *xid = (ccp)pool_copy((uccp)line_id_buf, atfmp->pool);
  atf_xprop(np, "xml:id", xid);
  atf_xprop(np, "strict", "0");
  atf_xprop(np, "type", "etcsl");  
  atf_xprop(np, "subt", mp->name);
  np->text = text;
}

/* Block and Line have a list of comments; if curr_line is non-NULL
 * attach to that; otherwise if abt->curr is a Block node attach to
 * that; otherwise error for now (need to check top-of-file attachment
 * and comment after $-line).
 * 
 */
void
atf_comment(Mloc l, char *text)
{
  if (!strncmp(text, "etcsl#", 6))
    etcsl_comment(l, text+6);

  if (curr_line)
    {
      if (!curr_line->cmts)
	{
	  curr_line->cmts = list_create(LIST_SINGLE);
	  memo_list(curr_line->cmts);
	}
      list_add(curr_line->cmts, text);
    }
  else if (abt->curr->user && (((Block*)abt->curr->user)->utype == N_U_BLOCK))
    {
      Block *bp = abt->curr->user;
      if (!bp->cmts)
	{
	  bp->cmts = list_create(LIST_SINGLE);
	  memo_list(bp->cmts);
	}
      list_add(bp->cmts, text);
    }
  else
    mesg_verr(&l, "comment is not preceded by Block or Line\n");
}
