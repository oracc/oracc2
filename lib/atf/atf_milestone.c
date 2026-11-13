#include <oraccsys.h>
#include "block.h"
#include "atf.h"

int m_trace = 0;
static int m_object_index = 0;
const char *curr_discourse;

/* strings assigned to type and subtype should be persistent, either
   from the Block struct or pool copied */

void
atf_milestone(Block *bp, char *rest)
{
  const char *type = NULL, *subtype = NULL, *text = NULL, *m_n = NULL;
  extern int m_label_col_index;
  char **toks = NULL;
  char *tokx[] = { NULL };
  if (rest)
    toks = vec_from_str(rest, NULL, NULL);
  else
    toks = tokx;

 if (m_trace)
   {
     int i;
     fprintf(stderr, "%s:%d: MILESTONE %s", bp->np->mloc->file, bp->np->mloc->line, bp->bt->name);
     for (i = 0; toks[i]; ++i)
       fprintf(stderr, "::%s", toks[i]);
     fputc('\n', stderr);
   }

 if (toks[0])
   {
     int i;
     for (i = 0; toks[i]; ++i)
       {
	 char *t = (char*)pool_copy((uccp)toks[i], atfmp->pool);
	 free(toks[i]);
	 toks[i] = t;
       }
     int bt_index = -1;
     if (!strcmp(toks[0], "locator"))
       bt_index = 1;
     else
       bt_index = 0;

     if (toks[bt_index])
       {
	 Blocktok *btokp = blocktok(toks[bt_index],strlen(toks[bt_index]));
	 if (btokp)
	   {
	     const char *sub_n = NULL;
	     type = "locator";
	     subtype = btokp->name;
	     switch (btokp->type)
	       {
	       case B_OBJECT:
		 if (toks[2])
		   {
		     sub_n = toks[2];
		     text = toks[3];
		   }
		 m_label_col_index = 0;
		 if (strcmp(subtype,"tablet") || sub_n)
		   update_mlabel(B_OBJECT, (uccp)(sub_n ? sub_n : type));
		 m_object_index = m_label_col_index;
		 break;
	       case B_SURFACE:
		 if (toks[++bt_index])
		   text = sub_n = toks[bt_index];
		 reset_mlabel();
		 update_mlabel(B_SURFACE,
			       (uccp)(btokp->nano ? btokp->nano : (sub_n ? sub_n : subtype)));
		 break;
	       case B_COLUMN:
		 update_mlabel(B_COLUMN, (uccp)(text = toks[++bt_index]));
		 break;
	       case B_DIVISION:
		 type = "division";
		 m_label_col_index = 0;
		 ++bt_index;
		 if (toks[bt_index])
		   {
		     subtype = toks[bt_index];
		     char *div = NULL;
		     if (!strcmp(toks[bt_index],"segment"))
		       div = "Seg";
		     else if (!strcmp(toks[bt_index],"paragraph"))
		       div = "Par";
		     else if (!strcmp(toks[bt_index],"exemplar"))
		       div = "Ex";
		     if (div)
		       {
			 ++bt_index;
			 if (toks[bt_index])
			   {
			     char buf[strlen(div)+strlen(toks[bt_index])+3];
			     sprintf(buf, "%s.%s,", div, toks[bt_index]);
			     m_n = (ccp)pool_copy((uccp)buf, atfmp->pool);
			     update_mlabel(B_SURFACE, (uccp)m_n);
			   }
			 else
			   mesg_verr(bp->np->mloc, "@division type incomplete (needs section info)");
		       }
		     else
		       {
			 char *tmp = vec_to_str(&toks[1], vec_len(&toks[1]), " ");
			 m_n = (ccp)pool_copy((uccp)tmp, atfmp->pool);
			 free(tmp);
			 update_mlabel(B_SURFACE, (uccp)m_n);
			 subtype = NULL;
		       }
		   }
		 else
		   mesg_verr(bp->np->mloc, "@division incomplete (needs division info)");
		 break;
	       case B_DISCOURSE:
		 type = "discourse";
		 curr_discourse = subtype = btokp->name;
		 if (!strcmp(subtype,"body"))
		   {
		     extern const char *default_discourse_level;
		     default_discourse_level = "preamble";
		   }
		 break;
	       case B_MILESTONE:
		 if (!strcmp("fragment",bp->bt->name) || !strcmp("cfragment",bp->bt->name))
		   {
		     if (toks[0])
		       {
			 type = "locator";
			 subtype = "fragment";
			 if (atfp->edoc == EDOC_COMPOSITE)
			   {
			     label_frag(bp->np,(uccp)toks[0]);
			     update_label(bp->np,etu_none);
			   }
			 else
			   {
			     char *buf = (char*)pool_alloc(strlen(toks[0])+strlen("frg.0"), atfmp->pool);
			     strcpy(buf,"frg.");
			     strcat(buf,toks[0]);
			     m_n = buf;
			     if ('c' == *bp->bt->name)
			       update_mlabel(B_OBJECT,(uccp)buf);
			     else
			       update_mlabel(B_SURFACE,(uccp)buf);
			   }
		       }
		     else
		       mesg_verr(bp->np->mloc, "@fragment/@cfragment must have frag indicator");
		   }
		 else
		   mesg_verr(bp->np->mloc, "unhandled MILESTONE %s", bp->bt->name);
		 break;
	       default:
		 mesg_verr(bp->np->mloc, "unhandled milestone block type %s", bp->bt->name);
		 break;
	       }
	   }
	 else
	   {
	     char *tmp = vec_to_str(&toks[bt_index], vec_len(&toks[bt_index]), " ");
	     subtype = (ccp)pool_copy((uccp)tmp, atfmp->pool);
	     m_label_col_index = 0;
	     free(tmp);
	     update_mlabel(B_SURFACE, (uccp)subtype);
	   }
       }
     else
       {
	   mesg_verr(bp->np->mloc, "@m locator incomplete (needs locator info)");
       }
     free(toks);
   }

 if (type)
   {
     atf_xprop(bp->np, "type", type);
     if (subtype)
       atf_xprop(bp->np, "subtype", subtype);
     if (m_n)
       {
	 atf_xprop(bp->np, "n", (ccp)pool_copy((uccp)m_n, atfmp->pool));
	 m_n = NULL;
       }
   }
 if (text)
   bp->np->text = (ccp)pool_copy((uccp)text, atfmp->pool);
}
