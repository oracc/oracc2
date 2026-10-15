#include <oraccsys.h>
#include "ax.h"
#include "ax_reader.h"

static char *axr_get_input(struct axrinfo *rip);
static void axr_set_input(struct axrinfo *rip);
static const char *axr_pqx(const char *atf);

struct axrinfo ri, *rip = &ri;

static const char *
axr_pqx(const char *atf)
{
  static char pqx[8] = { 0,0,0,0,0,0,0,0 };
  while (*atf && '&' != *atf)
    ++atf;
  if (*atf)
    {
      ++atf;
      strncpy(pqx, atf, 7);
      return pqx;
    }
  return NULL;
}

void
ax_read(const char *proj, enum axr_source from, void *list)
{
  char *atf;

  /*axr_init(rip);*/
  rip->proj = proj;
  rip->from = from;
  rip->list = list;
  rip->pool = pool_init();

  axr_set_input(rip);
  for (;;)
    {
      if (rip->stop || NULL == (atf = axr_get_input(rip)))
	break;
      if (!check_mode)
	ax_files(proj, axr_pqx(atf));
      ax_input(atf, rip->input_len);
      
    }
  /*axr_term(rip);*/
  pool_term(rip->pool);
  free(rip);
}

void
axr_init(struct axrinfo *rip)
{
  /*rip->filenames_list = list_create(LIST_SINGLE);*/
}

void
axr_term(struct axrinfo *rip)
{
#if 0
  pool_term(rip->pool);
  free(rip->charData_buf);
  rip->charData_buf = NULL;
  rip->charData_buflen = 0;
  if (rip->filenames_list)
    {
      list_free(rip->filenames_list,NULL);
      rip->filenames_list = NULL;
    }
#endif
}

static char *
axr_get_input(struct axrinfo *rip)
{
  size_t nread = 0;
  char *inputp;

  if (rip->this_one_is_done)
    {
      if (rip->more_sources)
	axr_set_input(rip);
      else
	rip->source = NULL;
    }

  if (!rip->source)
    return NULL;

  nread = fread(rip->input_buf, 1, AXR_BUFLEN, rip->source);
  if (ferror(rip->source))
    {
      fprintf(stderr,"read error\n");
      exit(-1);
    }
  rip->input_buf[nread] = '\0';
  inputp = rip->input_buf;

  nread -= inputp - rip->input_buf;
  rip->input_len = nread;

  if ((rip->this_one_is_done = feof(rip->source)))
    fclose(rip->source);

  return inputp;
}

static void
axr_set_input(struct axrinfo *rip)
{
  if (rip->from == axr_stdin)
    {
      rip->more_sources = 0;
      rip->source = stdin;
      rip->fname = (char*)pool_copy((ucp)"<stdin>", rip->pool);
    }
  else if (rip->from == axr_files)
    {
      rip->source = NULL;
      while (rip->more_sources)
	{
	  static char fname_buf[_MAX_PATH + 1];
	  char *fp = fgets(fname_buf,_MAX_PATH,stdin);
	  if (fp)
	    {
	      fp = fname_buf + strlen(fname_buf);
	      if ('\n' == fp[-1])
		fp[-1] = '\0';
	      if (NULL == (rip->source = fopen(fname_buf,"r"))/* && verbose*/)
		fprintf(stderr,"ax_reader: open failed on %s\n",fname_buf);
	      rip->more_sources = feof(stdin);
	      rip->fname = (char *)pool_copy((ucp)fname_buf, rip->pool);
	    }
	  else
	    {
	      rip->source = NULL;
	      rip->more_sources = 0;
	    }
	  if (rip->source)
	    break;
	}
    }
  else if (rip->from == axr_list)
    {
      if (rip->todo == NULL)
	{
	  rip->todo = rip->list;
	  if (NULL == rip->todo || NULL == *rip->todo)
	    {
	      rip->source = NULL;
	      rip->more_sources = 0;
	      rip->todo = NULL;
	      return;
	    }
	}
      rip->more_sources = 1;
      while (rip->more_sources)
	{
	  if (NULL == (rip->source = fopen(*rip->todo,"r"))/* && verbose*/)
	    fprintf(stderr,"runexpat: open failed on %s\n",*rip->todo);
	  else
	    rip->fname = (char *)pool_copy((ucp)*rip->todo, rip->pool);
	  rip->more_sources = (NULL != *++rip->todo);
	  if (!rip->more_sources)
	    rip->todo = NULL;
	  if (rip->source)
	    break;
	}
    }
}
