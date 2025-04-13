#include <oraccsys.h>
#include "ofp.h"

void
ofp_header(Ofp *o, const char *def)
{
#define arg (ccp)r->rows[i][1]
  if (access(def, R_OK))
    {
      fprintf(stderr, "ofpx: no such .def file %s\n", def);
      exit(1);
    }
  Roco *r = roco_load(def, 0, NULL, NULL, NULL, NULL);
  if (r)
    {
      int i;
      for (i = 0; i < r->nlines; ++i)
	{
	  struct ofpdefs_tab *otp = ofpdefs((ccp)r->rows[i][0], strlen((ccp)r->rows[i][0]));
	  if (otp)
	    {
	      switch (otp->v)
		{
		case ofpdefs_css:
		  o->h.css = arg;
		  break;
		case ofpdefs_data:
		  o->h.data = arg;
		  break;
		case ofpdefs_font:
		  o->h.font = arg;
		  break;
		case ofpdefs_label:
		  o->h.label = arg;
		  break;
		case ofpdefs_list:
		  o->h.list = arg;
		  break;
		case ofpdefs_mag:
		  o->h.mag = arg;
		  break;
		case ofpdefs_page:
		  o->h.page = arg;
		  break;
		case ofpdefs_path:
		  o->h.path = arg;
		  break;
		case ofpdefs_title:
		  o->h.title = arg;
		  break;
		default:
		  fprintf(stderr, "ofpx: unknown .def file value %d for key %s\n", otp->v, r->rows[i][0]);
		  break;
		}
	    }
	  else
	    fprintf(stderr, "ofpx: unknown .def file key %s\n", r->rows[i][0]);
	}
    }
#undef arg
}
