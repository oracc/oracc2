#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"

/* Block and Line have a list of comments; if curr_line is non-NULL
 * attach to that; otherwise if abt->curr is a Block node attach to
 * that; otherwise error for now (need to check top-of-file attachment
 * and comment after $-line).
 * 
 */
void
atf_comment(Mloc l, char *text)
{
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
