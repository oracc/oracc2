#include <oraccsys.h>
#include <tok.h>

/** XTF location writer routines */

/* Write the tloc lines that belong to a R(un) */
void
tlw_R(Trun *r)
{
  fprintf(r->o, "P\t%s\nY\t%s\n", pp(r->l->project), r->l->type);
}

/* Write a T(ext) tloc line and the K's that go with it */
void
tlw_T(Trun *r)
{
  if (!r->rs.F_done++)
    fprintf(r->o, "F\t%s\t%d\t%d\n",
	    loch_text(r)->file, loch_text(r)->andline_num, loch_text(r)->lastline_num);
  fprintf(r->o, "T\t%s\t%s\t%s\n",
	  loch_text(r)->text_project, loch_text(r)->text_id, loch_text(r)->text_name);
  if (loch_text(r)->keys)
    {
      int i;
      for (i = 0; i < loch_text(r)->nkeys; i+=3)
	fprintf(r->o, "K\t%s\t%06d/%s\n",
		loch_text(r)->keys[i], atoi(loch_text(r)->keys[i+2]), loch_text(r)->keys[i+1]);
    }
}

/* Write a L(ine) tloc line */
void
tlw_L(Trun *r)
{
  fprintf(r->o, "L\t%d\t%s\t%s\n",
	  loch_line(r)->line_num, loch_line(r)->line_id, loch_line(r)->line_label);
}

/* Write a W(ord) tloc line */
void
tlw_W(Trun *r)
{
  fprintf(r->o, "W\t%s\n", loch_word(r)->word_id);
}

/** CBD summaries.xml location/data writer routines */

void
tlw_G(Trun *r, const char *lang)
{
  fprintf(r->o, "G\t%s\n", lang);
}

void
tlw_A(Trun *r, const char *oid, const char *n)
{
  fprintf(r->o, "A\t%s\t%s\n", oid, n);
}
