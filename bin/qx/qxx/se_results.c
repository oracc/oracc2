#include <oraccsys.h>
#include "qx.h"

extern int gdf_flag;
static void vido_show_results(struct Datum *dp);
Vido *vp;

void
put_results(struct qxdata *qp, struct Datum *res)
{
  if (qp && qp->listfp)
    out_f = qp->listfp;
  
  if (show_count)
    printf("%lu\n",(unsigned long)res->count);
  else if (res->count)
    {
      if (xmldir)
	xmldir_results(xmldir,res->count);
      else
	{
	  if ('v' == id_prefix(res->l.l8p[0]->text_id))
	    vido_show_results(res);
	  else
	    show_results(res);
	}
    }
  else if (xmldir)
    xmldir_results(xmldir,res->count);

  if (out_f != stdout)
    {
      fclose(out_f);
      out_f = NULL;
    }
}

char
id_prefix(long int id)
{
  return 'v';
}

int
XidVal(long int id)
{
  if (idVal(id) >= X_FACTOR)
    return idVal(id)-X_FACTOR;
  else
    return idVal(id);
}

#define more_in_unit(curr,next) \
  (!s2									\
   && i+1 < dp->count							\
   && id_prefix((curr)->text_id) == id_prefix((next)->text_id)		\
   && XidVal((curr)->text_id) == XidVal((next)->text_id)		\
   && (curr)->unit_id == (next)->unit_id)

#define more_in_unit24(curr,next) \
  (!s2									\
   && i+1 < dp->count							\
   && id_prefix((curr)->text_id) == id_prefix((next)->text_id)		\
   && XidVal((curr)->text_id) == XidVal((next)->text_id)		\
   && ((struct location24*)curr)->sentence_id == ((struct location24*)next)->sentence_id)

void
show_results(struct Datum *dp)
{
  int i;
  switch (res_gran)
    {
    case g_not_set:
    case g_text:
    case g_record:
      for (i = 0; i < dp->count; ++i)
	{
	  struct location8 *l8p = dp->l.l8p[i];
	  if ('x' == id_prefix(l8p->text_id))
	    fprintf(out_f, "%s.x%07d\n", 
		    cbd_lang ? cbd_lang : se_curr_project, XidVal(l8p->text_id));
	  else
	    fprintf(out_f, "%c%06d\n", 
		    id_prefix(l8p->text_id), XidVal(l8p->text_id));
	}
      break;
    case g_field:
      for (i = 0; i < dp->count; ++i)
	{
	  struct location8 *l8p = dp->l.l8p[i];
	  fprintf(out_f, "%c%06d.%d\n",
		 id_prefix(l8p->text_id), XidVal(l8p->text_id),
		 l8p->unit_id);
	}
      break;
    case g_word:
    case g_grapheme:
      for (i = 0; i < dp->count; ++i)
	{
	  int last_id_printed = 0;
	  struct location8 *l8p = dp->l.l8p[i];
	  fprintf(out_f, "%c%06d.%d.%d", 
		 id_prefix(l8p->text_id), XidVal(l8p->text_id),
		 l8p->unit_id,
		 l8p->word_id);
	  while (more_in_unit(l8p, dp->l.l8p[i+1]))
	    {
#if 0
	      if (l8p->word_id != dp->l.l8p[i+1]->word_id)
#else
		if (dp->l.l8p[i+1]->word_id != last_id_printed)
#endif
		{
		  fprintf(out_f, ",%d",dp->l.l8p[++i]->word_id);
		  l8p = dp->l.l8p[i];
		  last_id_printed = l8p->word_id;
		}
	      else
		++i;
	    }
	  fputc('\n',out_f);
	}
      break;
    }
}

static const char *
set_tra_suffix(Two_bytes_s branch_id)
{
  return "";
}

static void
vido_show_results(struct Datum *dp)
{
  int i;
  switch (res_gran)
    {
    case g_not_set:
    case g_text:
    case g_record:
      for (i = 0; i < dp->count; ++i)
	{
	  struct location8 *l8p = dp->l.l8p[i];
	  fprintf(out_f, "%s\n", 
		  vido_get_id(vp,idVal(l8p->text_id)/*, vido_display_proj*/));
	}
      break;
    case g_field:
      for (i = 0; i < dp->count; ++i)
	{
	  struct location8 *l8p = dp->l.l8p[i];
	  fprintf(out_f, "%s.%d\n",
		  vido_get_id(vp,idVal(l8p->text_id)/*, vido_display_proj*/),
		  l8p->unit_id);
	}
      break;
    case g_word:
    case g_grapheme:
      if (dp->data_size == sizeof(struct location24))
	{
	  for (i = 0; i < dp->count; ++i)
	    {
	      struct location8 *l8p = dp->l.l8p[i];
	      const char *tra = "";
	      
	      if (return_index && !strcmp(return_index,"tra"))
		tra = set_tra_suffix(dp->l.l16p[i]->branch_id);
	      
	      fprintf(out_f, "%s%s.%d.%d", 
		      vido_get_id(vp,idVal(l8p->text_id)/*, vido_display_proj*/), 
		      tra,
		      l8p->unit_id,
		      l8p->word_id);
	      while (more_in_unit24(l8p, dp->l.l8p[i+1]))
		{
		  if (l8p->word_id != dp->l.l8p[i+1]->word_id)
		    {
		      /* FIXME: both the fprinted l8p[i]
			 here were l8p[++i] which must be
			 wrong--but with new wmapper this 
			 code is going to go away anyway */			
		      ++i;
		      fprintf(out_f, ",%d.%d",
			      dp->l.l8p[i]->unit_id,
			      dp->l.l8p[i]->word_id);
		      l8p = dp->l.l8p[i];
		    }
		  else
		    ++i;
		}
	      fputc('\n',out_f);
	    }
	}
      else
	{
	  for (i = 0; i < dp->count; ++i)
	    {
	      struct location8 *l8p = dp->l.l8p[i];
	      const char *tra = "";
	      
	      if (return_index && !strcmp(return_index,"tra"))
		tra = set_tra_suffix(dp->l.l16p[i]->branch_id);
	      
	      fprintf(out_f, "%s%s.%d.%d", 
		      vido_get_id(vp,idVal(l8p->text_id)/*, vido_display_proj*/), 
		      tra,
		      l8p->unit_id,
		      l8p->word_id);
	      while (more_in_unit(l8p, dp->l.l8p[i+1]))
		{
		  if (l8p->word_id != dp->l.l8p[i+1]->word_id)
		    {
		      fprintf(out_f, ",%d",dp->l.l8p[++i]->word_id);
		      l8p = dp->l.l8p[i];
		    }
		  else
		    ++i;
		}
	      fputc('\n',out_f);
	    }
	}
      break;
    }
}

