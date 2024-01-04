#include <oraccsys.h>
#include <tis.h>

static Memo *m_tis_info = NULL;
static Pool *p = NULL;

Tisp
tis_load(FILE *fp)
{
  Tisp t = NULL;
  if (fp)
    {
      t = hash_create(1024);
      size_t nbytes;
      unsigned char *lp;
      p = pool_init();
      struct tis_data *curr_tip = NULL;
      while ((lp = loadoneline(fp,&nbytes)))
	{
	  struct tis_data *tip = memo_new(m_tis_info);
	  tip->key = (char*)pool_copy(lp, p);
	  tip->ref = strchr(tip->key, '\t');
	  *tip->ref++ = '\0';
	  tip->cnt = strchr(tip->ref, '\t');
	  *tip->cnt++ = '\0';
	  tip->pct = strchr(tip->cnt, '\t');
	  *tip->pct++ = '\0';
	  tip->iss = strchr(tip->pct, '\t');
	  *tip->iss++ = '\0';
	  if (!strchr(tip->key, '|'))
	    {
	      curr_tip = tip;
	      hash_add(t, (ucp)tip->key, tip);
	    }
	  else
	    {
	      if (!curr_tip->subs)
		curr_tip->subs = list_create(LIST_SINGLE);
	      list_add(curr_tip->subs, tip);
	    }
	}
    }
  return t;
}

void
tis_load_init(void)
{
  if (!m_tis_info)
    m_tis_info = memo_init(sizeof(struct tis_data), 1024);
  if (!p)
    p = pool_init();
}

void
tis_load_term(Tisp t)
{
  if (m_tis_info)
    {
      memo_term(m_tis_info);
      m_tis_info = NULL;
    }
  if (t)
    hash_free(t, NULL);
}
