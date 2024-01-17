#include <oraccsys.h>
#include <runexpat.h>
#include <tok.h>

/* P229326.10.3 P229947.32.2.d */

/* For a number form to be a legal value it has to have a lower case
   vowel: this is a simple check but it is probably adequate, at least
   for the range of values in OGSL */
static int
n_value_ok(const char *form)
{
  while (*form)
    switch (*form)
      {
      case 'a':
      case 'e':
      case 'i':
      case 'u':
	return 1;
      case '@':
      case '~':
	return 0;
      default:
	++form;
      }
  return 0;
}

void
tok_g_sH(void *userData, const char *name, const char **atts)
{
  Trun *r = userData;
  
  if (r->rs.note_skip)
    return;
  
  if ('g' == name[0] && ':' == name[1])
    {
      if (!name[3]) /* P282496.56.2 X110014.5.6 */
	{
	  switch (name[2])
	    {
	    case 'w':
	      (void)trun_word_init(r);
	      /* By design rs.in_w is incremented/decremented after
		 calls to trun_word_init/trun_word_term */
	      ++r->rs.in_w;
	      /* r->rw->w holds state for words so we need to update
		 the reciprocal pointers after init */
	      break;
	    case 'd':
	      r->rw->role = 'd';
	      r->rw->roletext = (ccp)hpool_copy((uccp)findAttr(atts, "g:role"), r->p);
	      break;
	    case 'n':
	      r->rw->in_n = 1;
	      {
		const char *form = findAttr(atts, "form");
		gsb_add(r,
			name[2],
			form,
			findAttr(atts, "oid"),
			findAttr(atts, "g:sign"),
			NULL, NULL,
			r->rw->w->word_lang,
			findAttr(atts, "g:logolang"));
		if (n_value_ok(form))
		  gsb_value(r, form);
	      }
	      break;
	    case 'p':
	      gsb_add(r,
		      name[2],
		      findAttr(atts, "g:type"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      r->rw->w->word_lang,
		      findAttr(atts, "g:logolang"));
	      gsb_punct(r->rw, findAttr(atts, "g:type"));
	      r->rw->in_p = 1;
	      break;
	    case 'q':
	      gsb_add(r,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      findAttr(atts, "spoid"),
		      findAttr(atts, "spform"),
		      r->rw->w->word_lang,
		      findAttr(atts, "g:logolang"));
	      r->rw->in_q = 1;
	      break;
	    case 'c':
	      r->rw->in_c = 1;
	      gsb_add(r,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      r->rw->w->word_lang,
		      findAttr(atts, "g:logolang"));
	      break;
	    case 's':
	    case 'v':
	      if (!r->rw->in_n && !r->rw->in_p && !r->rw->in_q)
		gsb_add(r,
			name[2],
			NULL,
			findAttr(atts, "oid"),
			findAttr(atts, "g:sign"),
			NULL, NULL,
			r->rw->w->word_lang,
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
	    {
	      trun_word_init(r);
	      ++r->rs.in_w;
	    }
	}
    }
  else if ('n' == *name)
    {
      if (!strcmp(name, "n:w"))
	{
	  trun_word_init(r);
	  ++r->rs.in_w;
	}
      else if (!strcmp(name, "note:text"))
	r->rs.note_skip = 1;
    }
}

void
tok_g_eH(void *userData, const char *name)
{
  Trun *r = userData;

  if (r->rs.note_skip)
    {
      if (!strcmp(name, "note:text"))
	r->rs.note_skip = 0;
      (void)charData_retrieve();
      return;
    }
  
  if ('g' == name[0] && ':' == name[1])
    {
      if (!name[3])
	{
	  switch (name[2])
	    {
	    case 'w':
	      gsb_last(r);
	      tokw_G(r);
	      trun_word_reset(r);
	      --r->rs.in_w;
	      break;
	    case 'c':
	      r->rw->in_c = 0;
	      gsb_c_last(r->rw);
	      r->rw->wgp_c_index = -1;
	      break;
	    case 'd':
	      r->rw->role = '\0';
	      r->rw->roletext = "";
	      break;
	    case 'n':
	      r->rw->in_n = 0;
	      break;
	    case 'p':
	      r->rw->in_p = 0;
	      break;
	    case 'q':
	      r->rw->in_q = 0;
	      break;
	    case 's':
	      if (!r->rw->in_n && !r->rw->in_q)
		gsb_sign(r, charData_retrieve());
	      else
		(void)charData_retrieve();
	      break;
	    case 'v':
	      if (!r->rw->in_n/* && !r->rw->in_q*/)
		gsb_value(r, charData_retrieve());
	      else
		(void)charData_retrieve();
	      break;
	    case 'b':
	    case 'm':
	    case 'r':
	    case 'x':
	      (void)charData_retrieve();
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
	      tokw_G(r);
	      trun_word_reset(r);
	      --r->rs.in_w;
	    }
	  (void)charData_retrieve();
	}
    }
  else if ('n' == *name)
    {
      if (!strcmp(name, "n:w"))
	{
	  trun_word_reset(r);
	  --r->rs.in_w;
	}
      else
	(void)charData_retrieve();
    }
  else
    (void)charData_retrieve();
}
