#include <oraccsys.h>
#include <list.h>
#include <roco.h>
#include <xmlify.h>

/* FIXME: MEMORY MANAGEMENT -- keep roco list and delete or use static
   Memo for Rocos */

List *r_list;

#define uccp unsigned const char *

const char *roco_colorder = NULL;
const char *roco_format = NULL;
int roco_swap_axes = 0;
int roco_newline = 0;
int roco_xmlify = 1;

Roco *
roco_create(int rows, int cols)
{
  Roco *r = calloc(1, sizeof(Roco));
  r->nlines = rows;
  r->rows = calloc(rows, sizeof(const char **));
  int i;
  for (i = 0; i < r->nlines; ++i)
    r->rows[i] = calloc(cols, sizeof(const char *));
  return r;
}

void
roco_fields_row(Roco *r, const char **f)
{
  int i;
  for (i = 0; f[i]; ++i)
    r->rows[0][i] = (ucp)f[i];
  r->rows[0][i] = NULL;
  r->fields_from_row1 = 1;
}

Roco *
roco_load(const char *file, int fieldsr1,
	  const char *xtag, const char *rtag, const char *ctag, const char *class)
{
  Roco *r = calloc(1, sizeof(Roco));
  r->file = file;
  r->lines = loadfile_lines3((uccp)r->file, (size_t*)&r->nlines, &r->mem);
  r->rows = calloc(r->nlines, sizeof(unsigned char **));
  r->fields_from_row1 = fieldsr1;

  r->xmltag = (xtag ? xtag : "x");
  r->rowtag = (rtag ? rtag : "r");
  r->celtag = (ctag ? ctag : "c");
  r->class = class;

  int i;
  for (i = 0; i < r->nlines; ++i)
    {
      int ncol = 1;
      unsigned char *s = r->lines[i];
      while (*s)
	 {
	  if ('\t' == *s)
	    ++ncol;
	  ++s;
	}
      if (ncol > r->maxcols)
	r->maxcols = ncol;
      r->rows[i] = calloc(ncol+1, sizeof(unsigned char *));
      int col;
      for (col = 0, s = r->lines[i]; *s; ++col)
	{
	  if ('\t' == *s)
	    r->rows[i][col] = (unsigned char *)"";
	  else
	    r->rows[i][col] = s;
	  while (*s && '\t' != *s)
	    ++s;
	  if ('\t' == *s)
	    *s++ = '\0';
	}

      while (col < ncol)
	r->rows[i][col++] = (unsigned char *)"";
	
      r->rows[i][col] = NULL;
    }
  return r;
}

void
roco_hash_hash(Hash *h, Roco *r)
{
  size_t i;
  for (i = 0; i < r->nlines; ++i)
    {
      if ('#' != *r->rows[i][0])
	{
	  if (!strcmp((ccp)r->rows[i][0], ".include"))
	    roco_hash_hash(h, roco_load1((ccp)r->rows[i][1]));      
	  else
	    hash_add(h, r->rows[i][0], r->rows[i][1]);
	}
    }
}

Hash *
roco_hash(Roco *r)
{
  Hash *h = hash_create(r->nlines/2);
  roco_hash_hash(h, r);
  return h;
}

static void
roco_hash_hash_r(Hash *h, Roco *r)
{
  size_t i;
  for (i = 0; i < r->nlines; ++i)
    {
      if ('#' != *r->rows[i][0])
	{
	  if (!strcmp((ccp)r->rows[i][0], ".include"))
	    roco_hash_hash(h, roco_load1((ccp)r->rows[i][1]));      
	  else
	    hash_add(h, r->rows[i][0], r->rows[i]);
	}
    }
}

Hash *
roco_hash_r(Roco *r)
{
  Hash *h = hash_create(r->nlines/2);
  roco_hash_hash_r(h, r);
  return h;
}

void
roco_field_indexes(Roco *r)
{
  if (r->fields_from_row1)
    {
      r->fields = hash_create(128);
      int i;
      /* index from 1 because 0 == NULL ptr */
      for (i = 0; i < r->maxcols; ++i)
	hash_add(r->fields, r->rows[0][i], (void*)(uintptr_t)(i+1));
    }
}

static const char *
roco_co_fo(void)
{
  char buf[strlen(roco_colorder) * 4], *b = buf;
  const char *s;
  for (s = roco_colorder; *s; ++s)
    {
      *b++ = '%';
      *b++ = *s;
      if (s[1])
	*b++ = '\t';
    }
  *b++ = '\n';
  *b = '\0';
  return strdup(buf);
}

const char *
roco_z_format(List *lp, Roco *r)
{
  if (r->fields_from_row1)
    {
      int i, ip[r->maxcols+1];
      if (!r->fields)
	roco_field_indexes(r);
      for (i = 0; i < r->maxcols; ++i)
	ip[i] = ((int)(uintptr_t)hash_find(r->fields, (uccp)r->rows[0][i]));
      ip[i] = 0;
      char **zp = (char**)list2array(lp);
      for (i = 0; zp[i]; ++i)
	{
	  char buf[strlen(zp[i])+1];
	  strcpy(buf, zp[i]);
	  char *eq = strchr(buf, '=');
	  if (eq)
	    {
	      *eq++ = '\0';
	      void
		*v1 = hash_find(r->fields, (uccp)buf),
		*v2 = hash_find(r->fields, (uccp)eq);
	      if (v1 && v2)
		{
		  int
		    zl = (int)(uintptr_t)v1,
		    zr = (int)(uintptr_t)v2;
		  ip[zl-1] = zr;
		  ip[zr-1] = zl;
		}
	      else
		{
		  if (!v1)
		    fprintf(stderr, "roco_z_format: field %s not in field indexes\n", buf);
		  if (!v2)
		    fprintf(stderr, "roco_z_format: field %s not in field indexes\n", eq);
		  return NULL;
		}
	    }
	  else
	    {
	      void *v1 = hash_find(r->fields, (uccp)buf);
	      if (v1)
		{
		  int z1 = (int)(uintptr_t)v1;
		  int i;
		  /* move everything before z1 one slot to the right */
		  for (i = z1-1; i > 0; --i)
		    ip[i] = ip[i-1];
		  ip[0] = z1;
		}
	      else
		{
		  fprintf(stderr, "roco_z_format: field %s not in field indexes\n", buf);
		  return NULL;
		}
	    }
	}
      char fmt[(i * 7) + 1];
      for (i = 0, *fmt = '\0'; ip[i]; ++i)
	{
	  strcat(fmt, "%");
	  strcat(fmt, itoa(ip[i])); /* -R indexes from 1 because %0 means pad with 1 zero */
	  if (ip[i+1])
	    strcat(fmt, "\t");
	  else
	    strcat(fmt, "\n");
	}
      return strdup(fmt);
    }
  else
    {
      fprintf(stderr, "roco_z_format: need fields_from_row1 == 1\n");
      return NULL;
    }
}

void
roco_empty_row(Roco *r)
{
  char buf[(r->maxcols-r->joiner)+1];
  memset(buf, '\t', r->maxcols-r->joiner);
  buf[r->maxcols-r->joiner] = '\0';
  r->empty_row = strdup(buf);
}

static void
roco_join(FILE *fp, Roco *r, int i)
{
  if (!i)
    {
      Roco *jr;
      for (jr = list_first(r_list); jr; jr = list_next(r_list))
	{
	  int j;
	  for (j = 1; jr->rows[0][j]; ++j)
	    {
	      fputc('\t', fp);
	      if (*jr->rows[0][j])
		fputs((const char *)jr->rows[0][j], fp);
	    }
	}
    }
  else
    {
      Roco *jr;
      for (jr = list_first(r_list); jr; jr = list_next(r_list))
	{
	  int j;
	  if (jr->hdata)
	    {
	      const char **row = hash_find(jr->hdata, r->rows[i][0]);
	      if (row)
		{
		  for (j = 1; row[j]; ++j)
		    {
		      fputc('\t', fp);
		      if (*row[j])
			fputs(row[j], fp);
		    }
		}
	      else
		fputs(jr->empty_row, fp);
	    }
	}      
    }
}

void
roco_write(FILE *fp, Roco *r)
{
  size_t i;

  if (roco_colorder)
    roco_format = roco_co_fo();
  
  for (i = 0; i < r->nlines; ++i)
    {
      if (roco_format && (i || !r->row1_literal))
	roco_row_format(fp, (const unsigned char **)r->rows[i]);
      else
	{
	  int j;
	  for (j = 0; r->rows[i][j] != NULL; ++j)
	    {
	      if (j)
		fputc('\t', fp);
	      if (r->linkcells)
		{
		  Link *lp;
		  for (lp = (Link*)r->rows[i][j]; lp; lp = lp->next)
		    {
		      fputs((const char *)lp->data, fp);
		      if (lp->next)
			fputc('#', fp);
		    }
		}
	      else
		{
		  if (*r->rows[i][j])
		    fputs((const char *)r->rows[i][j], fp);
		}
	    }

	  if (r_list)
	    roco_join(fp, r, i);

	  if (j)
	    fputc('\n', fp);
	}
    }

  if (roco_colorder)
    free((void*)roco_format);
}

Roco *
roco_swap(Roco *r)
{
  Roco *s = roco_create(r->maxcols, r->nlines);
  int i, j;
  for (i = 0; i < r->nlines; ++i)
    {
      for (j = 0; r->rows[i][j]; ++j)
	s->rows[j][i] = r->rows[i][j];
    }
  return s;
}

void
roco_reorder(Roco *r, int left, int right)
{
  int i;
  for (i = 0; r->rows[i]; ++i)
    {
      ucp tmp = r->rows[i][left];
      r->rows[i][left] = r->rows[i][right];
      r->rows[i][right] = tmp;
    }
}
