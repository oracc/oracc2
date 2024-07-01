#include <oraccsys.h>
#include <atf2utf.h>
#include <charsets.h>
#include "qx.h"

struct Datum result;
struct token *toks = NULL;
int ntoks;

extern struct expr_rules *ret_type_rules;
extern struct Datum evaluate(struct token *toks, int fcode, int lmask,
			     struct token **lastused);

static int same_loc8(struct location8 *a, struct location8 *b);
static void n_uniq_loc8(struct Datum *dp);
static void uniq (struct Datum *dp);
static void pretrim_setup(void);

long
result_count(void)
{
  return result.count;
}

int
se_run_qx(struct qxdata *qp, struct sdata *sdp)
{
  qp->srchv = (const char**)vec_from_str((char*)pool_copy((ucp)qp->bar,qp->p), NULL, NULL);
  return se_run(qp, sdp);
}

int
se_run(struct qxdata *qp, struct sdata *sdp)
{
  int ntoks = 0;

  const char **tokv = qp->srchv ? qp->srchv : (const char **)(qp->argv+qp->argc);

  atf2utf_init();
  charsets_init();
  langtag_init();
  tokinit();

  if (pretrim_file || pretrim_args)
    pretrim_setup();

  if (show_tokens)
    {
      toks = tokenize(tokv,&ntoks);
    }
  else
    {
      if (qp->any_index)
	{
	  return se_any(qp, sdp);
	}
      else if (project && arg_index)
	{
	  const char **toklist = NULL;
	  toklist = anytoks(project, arg_index, tokv);
	  toks = tokenize(toklist,&ntoks);
	  run_search(toks);
	  put_results(qp, &result);
	  if (out_f && out_f != stdout)
	    {
	      fclose(out_f);
	      out_f = NULL;
	    }
	  sdata.count = (int)result.count;
	  qx_count_file(&sdata);
	}
      else
	{
	  toks = tokenize(tokv,&ntoks);
	  run_search(toks);
	  put_results(qp, &result);
	  if (out_f && out_f != stdout)
	    {
	      fclose(out_f);
	      out_f = NULL;
	    }
	  sdata.count = (int)result.count;
	  qx_count_file(&sdata);
	}
    }

  if (pretrim)
    {
      hash_free(pretrim, NULL);
      pretrim = NULL;
      free(pretrim_lines);
      free(pretrim_content);
      pretrim_content = NULL;
      pretrim_lines = NULL;
      if (pretrim_args)
	{
	  list_free(pretrim_args, NULL);
	  pretrim_args = NULL;
	}
    }
  return 0;
}

void
run_search(struct token*toks)
{
  binop_init();
  binop24_init();

  result = evaluate(toks, -1, lm_any, NULL);

#if 0
  progress("se: result.count == %lu\n", (unsigned long)result.count);
#endif
  
  if (do_uniq && result.count > 1)
    {
      if (res_gran == g_word || res_gran == g_grapheme)
	n_uniq_loc8(&result);
      else
	uniq(&result);
#if 0
      progress("se: post-uniq result.count == %lu\n", (unsigned long)result.count);
#endif
    }
}

static int
same_loc8(struct location8 *a, struct location8 *b)
{
  return 
    a->text_id == b->text_id
    && a->unit_id == b->unit_id
    && a->word_id == b->word_id;
}

static void
n_uniq_loc8(struct Datum *dp)
{
  struct location8 **lp = malloc(dp->count * sizeof(struct location8*));
  struct location8 **rp = malloc(dp->count * sizeof(struct location8*));
  int i, newcount = 0;
  /* always have to copy the first one ... */
  lp[0] = dp->l.l8p[0];
  if (dp->r.l8p)
    rp[0] = dp->r.l8p[0];
  ++newcount;
  for (i = 0; i < dp->count; ++i)
    if (i && !same_loc8(dp->l.l8p[i-1],dp->l.l8p[i]))
      {
	lp[newcount] = dp->l.l8p[i];
	if (dp->r.l8p)
	  rp[newcount] = dp->r.l8p[i];
	++newcount;
      }
  if (newcount < dp->count)
    {
      dp->l.l8p = lp;
      if (dp->r.l8p)
	dp->r.l8p = rp;
      dp->count = newcount;
    }
}

static void
uniq (struct Datum *dp)
{
  size_t end = 0, last = dp->count - 1, insert_point;

  while (end < last 
	 && (dp->l.l8p[end]->text_id != dp->l.l8p[end+1]->text_id
	     && (textresult
		 || dp->l.l8p[end]->unit_id != dp->l.l8p[end+1]->unit_id)))
    ++end;

  if (end < last) /* there are duplicates */
    {
      /* insert point is at the first of a series of duplicates:
	 it is the last duplicate that gets copied to insert_point
	 when the skip duplicates loop breaks */	 
      insert_point = end;
      while (1)
	{
	  size_t lastend = end;
	  /* skip duplicates: [NO LONGER] guaranteed to be true at least once */
	  while (end < last && 
		 (dp->l.l8p[end]->text_id == dp->l.l8p[end+1]->text_id
		  && (textresult 
		      || dp->l.l8p[end]->unit_id == dp->l.l8p[end+1]->unit_id)))
	    ++end;

	  if (end == lastend)
	    {
	      dp->l.l8p[insert_point] = dp->l.l8p[end];
	      if (NULL != dp->r.l8p)
		dp->r.l8p[insert_point] = dp->r.l8p[end];
	      ++insert_point;
	      ++end;
	    }
	  else
	    {
	      /* move non-duplicates backwards */
	      while (end < last 
		     && (dp->l.l8p[end]->text_id != dp->l.l8p[end+1]->text_id
			 && (textresult
			     || dp->l.l8p[end]->unit_id != dp->l.l8p[end+1]->unit_id)))
		{
		  dp->l.l8p[insert_point] = dp->l.l8p[end];
		  if (NULL != dp->r.l8p)
		    dp->r.l8p[insert_point] = dp->r.l8p[end];
		  ++insert_point;
		  ++end;
		}
	    }  
	  
	  if (end == last) /* no more duplicates possible */
	    {
	      dp->l.l8p[insert_point] = dp->l.l8p[end];
	      if (NULL != dp->r.l8p)
		dp->r.l8p[insert_point] = dp->r.l8p[end];
	      ++insert_point;
	      break;
	    }
	}
      dp->count = insert_point;
    }
}

static void
pretrim_setup(void)
{
  unsigned char *p;
  static int true = 1;
  pretrim = hash_create(1000);
  if (pretrim_args)
    for (p = list_first(pretrim_args); p; p = list_next(pretrim_args))
      hash_add(pretrim, p, &true);
  else
    {
      int i;
      pretrim_lines = loadfile_lines3((unsigned char *)pretrim_file, NULL, &pretrim_content);
      for (i = 0; pretrim_lines[i]; ++i)
	hash_add(pretrim, pretrim_lines[i], &true);
    }
}
