#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

void
tok_g_sH(void *userData, const char *name, const char **atts)
{
  Trun *r = userData;
  
  if ('g' == name[0] && ':' == name[1])
    {
      if (!name[3])
	{
	  switch (name[2])
	    {
	    case 'w':
	      (void)trun_word_init(r);
	      break;
	    case 'd':
	      r->rs.role = 'd';
	      r->rs.roletext = (ccp)hpool_copy((uccp)findAttr(atts, "g:role"), r->p);
	      break;
	    case 'n':
	      r->rs.in_n = 1;
	      gsb_add(r,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      r->rw.w->word_lang,
		      findAttr(atts, "g:logolang"));
	      gsb_value(r, findAttr(atts, "form"));
	      break;
	    case 'p':
	      gsb_add(r,
		      name[2],
		      findAttr(atts, "g:type"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      r->rw.w->word_lang,
		      findAttr(atts, "g:logolang"));
	      gsb_punct(&r->rw, findAttr(atts, "g:type"));
	      r->rs.in_p = 1;
	      break;
	    case 'q':
	      gsb_add(r,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      findAttr(atts, "spoid"),
		      findAttr(atts, "spform"),
		      r->rw.w->word_lang,
		      findAttr(atts, "g:logolang"));
	      r->rs.in_q = 1;
	      break;
	    case 'c':
	      r->rs.in_c = 1;
	      gsb_add(r,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      r->rw.w->word_lang,
		      findAttr(atts, "g:logolang"));
	      break;
	    case 's':
	    case 'v':
	      if (!r->rs.in_n && !r->rs.in_p && !r->rs.in_q)
		gsb_add(r,
			name[2],
			NULL,
			findAttr(atts, "oid"),
			findAttr(atts, "g:sign"),
			NULL, NULL,
			r->rw.w->word_lang,
			findAttr(atts, "g:logolang"));
	      break;
	    case 'b':
	    case 'r':
	      break;
	    default:
	      break;
	    }
	}
      else
	{
	  if (!strcmp(name, "g:nonw"))
	    trun_word_init(r);
	}
    }
}

void
tok_g_eH(void *userData, const char *name)
{
  Trun *r = userData;

  if ('g' == name[0] && ':' == name[1])
    {
      if (!name[3])
	{
	  switch (name[2])
	    {
	    case 'w':
	      gsb_last(r);
	      gsb_show(stdout, r);
	      trun_word_reset(&r->rw);
	      break;
	    case 'c':
	      r->rs.in_c = 0;
	      gsb_c_last(&r->rw);
	      r->rs.curr_c_wgp = NULL;
	      break;
	    case 'd':
	      r->rs.role = '\0';
	      r->rs.roletext = "";
	      break;
	    case 'n':
	      r->rs.in_n = 0;
	      break;
	    case 'p':
	      r->rs.in_p = 0;
	      break;
	    case 'q':
	      r->rs.in_q = 0;
	      break;
	    case 's':
	      if (!r->rs.in_n && !r->rs.in_q)
		gsb_sign(r, charData_retrieve());
	      break;
	    case 'v':
	      if (!r->rs.in_n && !r->rs.in_q)
		gsb_value(r, charData_retrieve());
	      break;
	    case 'b':
	    case 'r':
	      break;
	    default:
	      (void)charData_retrieve();
	      break;
	    }
	}
      else
	{
	  if (!strcmp(name, "g:nonw"))
	    {
	      gsb_last(r);
	      gsb_show(stdout, r);
	      trun_word_reset(&r->rw);
	      r->rw.w->word_lang = "";
	    }
	  else
	    (void)charData_retrieve();
	}
    }
  else
    (void)charData_retrieve();
}
