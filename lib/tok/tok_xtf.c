#include <tok.h>

void
g_xtf_sH(void *userData, const char *name, const char **atts)
{
  Trun *r = userData;
  
  if ('g' == name[0] && ':' == name[1])
    {
      if (!name[3])
	{
	  switch (name[2])
	    {
	    case 'w':
	      gswp = gsb_word_init(gi);
	      gswp->lang = (ccp)hpool_copy((uccp)get_xml_lang(atts), gswp->in->p);
	      gswp->form = (ccp)hpool_copy((uccp)findAttr(atts, "form"), gswp->in->p);
	      break;
	    case 'd':
	      gswp->in->role = 'd';
	      gswp->in->roletext = (ccp)hpool_copy((uccp)findAttr(atts, "g:role"), gswp->in->p);
	      break;
	    case 'n':
	      gswp->in->in_n = 1;
	      gsb_add(gswp,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      gswp->lang,
		      findAttr(atts, "g:logolang"));
	      gsb_value(gswp, findAttr(atts, "form"));
	      break;
	    case 'p':
	      gsb_add(gswp,
		      name[2],
		      findAttr(atts, "g:type"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      gswp->lang,
		      findAttr(atts, "g:logolang"));
	      gsb_punct(gswp, findAttr(atts, "g:type"));
	      gswp->in->in_p = 1;
	      break;
	    case 'q':
	      gsb_add(gswp,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      findAttr(atts, "spoid"),
		      findAttr(atts, "spform"),
		      gswp->lang,
		      findAttr(atts, "g:logolang"));
	      gswp->in->in_q = 1;
	      break;
	    case 'c':
	      gswp->in->in_c = 1;
	      gsb_add(gswp,
		      name[2],
		      findAttr(atts, "form"),
		      findAttr(atts, "oid"),
		      findAttr(atts, "g:sign"),
		      NULL, NULL,
		      gswp->lang,
		      findAttr(atts, "g:logolang"));
	      break;
	    case 's':
	    case 'v':
	      if (!gswp->in->in_n && !gswp->in->in_p && !gswp->in->in_q)
		gsb_add(gswp,
			name[2],
			NULL,
			findAttr(atts, "oid"),
			findAttr(atts, "g:sign"),
			NULL, NULL,
			gswp->lang,
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
	      gswp = gsb_word_init(gi);
	    }
	}
    }
}

void
g_xtf_eH(void *userData, const char *name)
{
  Trun *r = userData;

  if ('g' == name[0] && ':' == name[1])
    {
      if (!name[3])
	{
	  switch (name[2])
	    {
	    case 'w':
	      gsb_last(gswp);
	      gsb_show(stdout, gswp, form_with_w);
	      gsb_word_reset(gswp);
	      break;
	    case 'c':
	      gswp->in->in_c = 0;
	      gsb_c_last(gswp);
	      gswp->curr_c_wgp = NULL;
	      break;
	    case 'd':
	      gswp->in->role = '\0';
	      gswp->in->roletext = "";
	      break;
	    case 'n':
	      gswp->in->in_n = 0;
	      break;
	    case 'p':
	      gswp->in->in_p = 0;
	      break;
	    case 'q':
	      gswp->in->in_q = 0;
	      break;
	    case 's':
	      if (!gswp->in->in_n && !gswp->in->in_q)
		gsb_sign(gswp, charData_retrieve());
	      break;
	    case 'v':
	      if (!gswp->in->in_n && !gswp->in->in_q)
		gsb_value(gswp, charData_retrieve());
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
	      gsb_last(gswp);
	      gsb_show(stdout, gswp, form_with_w);
	      gsb_word_reset(gswp);
	      gswp->lang = "";
	    }
	  else
	    (void)charData_retrieve();
	}
    }
  else
    (void)charData_retrieve();
}
