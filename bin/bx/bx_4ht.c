#include <oraccsys.h>
#include <runexpat.h>
#include "bx.h"
#include "bbl.h"

extern Bbl *bbl;
static int tex4ht_found = 0;
static Bblentry *curr_ep = NULL;

void
bx4ht_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "meta"))
    {
      const char *content = findAttr(atts, "content");
      if (strstr(content, "TeX4ht"))
	tex4ht_found = 1;
    }
  else if ('d' == *name && !name[2] && ('t' == name[1] || 'd' == name[1]))
    {
      const char *class = findAttr(atts, "class");
      if (!strcmp(class, "thebibliography"))
	{
	  const char *id = findAttr(atts, "id");
	  if ('t' == name[1])
	    {
	      id = strchr(id, '-');
	      ++id;
	      if (!(curr_ep = hash_find(bbl->ehash, (uccp)id)))
		{
		  fprintf(stderr, "bx4ht id %s not found in bbl refs", id);
		}
	      /* else: we have just found curr_ep via h4t_ref which
		 gets set when reading the .bbl file */
	    }
	  else if (curr_ep)
	    {
	      curr_ep->h4t_bib = (ccp)pool_copy((uccp)id, p);
	      curr_ep = NULL;
	    }
	}
    }
}

void
bx4ht_eH(void *userData, const char *name)
{
  
}

void
bx_4ht(const char *tex4ht_file)
{
  const char *f[2];
  f[0] = tex4ht_file;
  f[1] = NULL;
  runexpat(i_list, f, bx4ht_sH, bx4ht_eH);
  if (!tex4ht_found)
    fprintf(stderr, "no meta content=TeX4ht found in supposed 4ht input\n");
}

const char *
bx4ht_ref(const char *r)
{
  size_t ulen;
  wchar_t *w = utf2wcs((uccp)r, &ulen);
  if (ulen != strlen(r))
    {
      char buf[(2*ulen)+1];
      int i, dest;
      for (i = dest = 0; i < ulen; ++i)
	{
	  if (w[i] < 128)
	    buf[dest++] = w[i];
	  else
	    {
	      buf[dest++] = '_';
	      buf[dest++] = '_';
	    }
	}
      buf[dest] = '\0';
      return (ccp)pool_copy((uccp)buf, p);
    }
  else
    return r;
}

 
