#include <oraccsys.h>
#include "ax.h"
#include "ax_reader.h"

extern int verbose;

static int axr_backup_input(long n);
static char *axr_get_atf(struct axrinfo *rip);
static int axr_get_input(struct axrinfo *rip);
static int axr_grow_input(struct axrinfo *rip);
static int axr_read_input(struct axrinfo *rip);
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
  rip->input_buf_alloced = AXR_BUFLEN;
  rip->input_buf = malloc(rip->input_buf_alloced);
  axr_set_input(rip);
  if (axr_get_input(rip))
    {
      extern ssize_t last_atflineno;
      for (;;)
	{
	  if (rip->stop)
	    break;
	  else if ((atf = axr_get_atf(rip)))
	    {
	      if (!check_mode && proj)
		ax_files(proj, axr_pqx(atf));
	      else
		ax_files_static();
	      atf_set_lineno(last_atflineno);
	      ax_input(atf, rip->atf_len);
	    }
	  else
	    break;
	}
    }
  /*axr_term(rip);*/
  pool_term(rip->pool);
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
axr_amp_line(struct axrinfo *rip, unsigned const char *p)
{
  while ((p = (uccp)strchr((ccp)p, '&')))
    {
      /* find preceding '\n' followed by optional BOM */
      unsigned const char *b = p;
      while (b > (uccp)rip->input_buf && '\n' != b[-1])
	--b;
      if (p == b
	  || p - b == 1
	  || (p - b == 2 && 0xfe == b[0] && 0xff == b[1])
	  || (p - b == 3 && 0xef == b[0] && 0xbb == b[1] && 0xbf == b[2]))
	return (char*)p;
      else
	++p; /* this '&' was not at SOL */
    }
  return NULL;
}


/* axr_set_input and axr_get_input must be called before this is invoked */
static char *
axr_get_atf(struct axrinfo *rip)
{
 amp_init:
  if (!rip->amp_ptr)
    {
      rip->amp_ptr = axr_amp_line(rip, (uccp)rip->input_buf);
    }
  else if (rip->end_ptr)
    {
      rip->amp_ptr = rip->end_ptr;
      rip->amp_ptr[0] = rip->save0;
      rip->amp_ptr[1] = rip->save1;
      rip->save0 = rip->save1 = '\0';
    }
  else
    {
      if (rip->this_one_is_done)
	{
	  if (!axr_get_input(rip))
	    return NULL;
	  rip->amp_ptr = NULL;
	  goto amp_init;
	}
      else
	{
	  /* if end_ptr is NULL but file is not finished there is something wrong */
	  fprintf(stderr, "axr_get_atf: internal error: end_ptr==NULL but file is not finished\n");
	  return NULL;
	}
    }
 amp_cont:
  if (rip->amp_ptr)
    {
      rip->end_ptr = axr_amp_line(rip, (uccp)rip->amp_ptr+1);
      if (rip->end_ptr)
	{
	  rip->save0 = rip->end_ptr[0];
	  rip->save1 = rip->end_ptr[1];
	  rip->end_ptr[0] = rip->end_ptr[1] = '\0';
	  rip->atf_len = rip->end_ptr - rip->amp_ptr;
	  return rip->amp_ptr;
	}
      else
	{
	  if (rip->this_one_is_done)
	    {
	      /* the input buffer contains the entire final ATF chunk
		 of the file; leave end_ptr == NULL */
#if 1
	      rip->input_buf[rip->input_len+1] = '\0';
	      rip->atf_len = strlen(rip->amp_ptr);
#else
	      rip->end_ptr = rip->input_buf + rip->input_len;
	      rip->end_ptr[1] = '\0';
#endif
	      return rip->amp_ptr;
	    }
	  else if (rip->amp_ptr == rip->input_buf)
	    {
	      /* the input buffer is not big enough to hold this ATF chunk */
	      axr_backup_input(- rip->input_len);
	      if (axr_grow_input(rip)) /* this does a new axr_get_input() */
		goto amp_init;
	    }
	  else
	    {
	      if (!axr_backup_input(- strlen(rip->amp_ptr)))
		{
		  if (axr_get_input(rip))
		    {
		      rip->amp_ptr = axr_amp_line(rip, (uccp)rip->input_buf);
		      goto amp_cont;
		    }
		}
	    }
	}
    }
  return NULL;
}

/* Back up the file position indicator to the last ATF text in
   input_buf because it may be incomplete */
static int
axr_backup_input(long n)
{
  return fseek(rip->source, n, SEEK_CUR);
}

static int
axr_grow_input(struct axrinfo *rip)
{
  rip->input_buf_alloced *= 2;
  rip->input_buf = realloc(rip->input_buf, rip->input_buf_alloced);
  rip->amp_ptr = rip->end_ptr = 0;
  return axr_read_input(rip);
}

static int
axr_read_input(struct axrinfo *rip)
{
  size_t nread = fread(rip->input_buf, 1, rip->input_buf_alloced-3, rip->source);
  if (ferror(rip->source))
    {
#if 1
      perror(rip->fname);
#else
      fprintf(stderr,"ax_reader: read error on %s\n", rip->fname);
#endif
      return 0;
    }
  if ((rip->this_one_is_done = feof(rip->source)))
    {
      fclose(rip->source);
      if ('\n' != rip->input_buf[nread-1])
	rip->input_buf[nread++] = '\n';
    }
  rip->input_buf[nread] = rip->input_buf[nread+1] = '\0';
  rip->input_len = nread;
  return 1;
}

/* if there is more input to work on return 1; otherwise return 0 */
static int
axr_get_input(struct axrinfo *rip)
{
  if (rip->this_one_is_done)
    {
      if (rip->more_sources)
	axr_set_input(rip);
      else
	rip->source = NULL;
    }

  if (!rip->source)
    return 0;

  if (!axr_read_input(rip))
    return 0;

  return rip->input_len ? 1 : 0;
}

static void
axr_set_input(struct axrinfo *rip)
{
  if (rip->from == axr_stdin)
    {
      rip->more_sources = 0;
      rip->source = stdin;
      rip->fname = (char*)pool_copy((ucp)"<stdin>", rip->pool);
      atf_set_file(rip->fname);
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
	      atf_set_file(rip->fname);
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
	    fprintf(stderr,"ax_reader: open failed on %s\n",*rip->todo);
	  else
	    {
	      rip->fname = (char *)pool_copy((ucp)*rip->todo, rip->pool);
	      atf_set_file(rip->fname);
	      if (verbose)
		fprintf(stderr, "ax_reader: opened %s\n", rip->fname);
	    }
	  rip->more_sources = (NULL != *++rip->todo);
	  if (!rip->more_sources)
	    rip->todo = NULL;
	  if (rip->source)
	    break;
	}
    }
}
