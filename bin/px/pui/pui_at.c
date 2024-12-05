#include <oraccsys.h>
#include <xpd.h>
#include "../isp/isp.h"
#include "../px.h"
#include "../xslt_if.h"

static int
active_pages(Isp *ip)
{
  int psize = atoi(ip->psiz);
  int pmax = 0;
  if (ip->glos && !ip->glosdata.xis)
    {
      if (ip->glosdata.let && strcmp(ip->glosdata.let, "entry_ids"))
	pmax = atoi(ip->glosdata.lmax);
      else
	pmax = atoi(ip->glosdata.emax);
    }
  else
    pmax = ip->zmax;
  int npages =  pmax / psize;
  if (pmax % psize)
    ++npages;
  return npages;
}

void
pui_at_pager_class(Isp *ip, FILE *fp)
{
  fputs("bborder-top", fp);
  fputc(' ', fp);
  fputs(ip->from, fp);
  fputc(' ', fp);
  if (ip->item)
    fputs("item", fp);
  else
    fputs("page", fp);
  fputc(' ', fp);
  if (ip->data)
    fputs(ip->data, fp);
  if (ip->item)
    {
      if (!strcmp(ip->data, "dglo"))
	{
	  if (ip->glosdata.ent)
	    fputs(" iart", fp);
	  else
	    fputs(" itxt", fp);
	}
      else
	fputs(" itxt", fp);
    }
  else
    {
      if (ip->show)
	{
	  fputc(' ', fp);
	  fputs(ip->show, fp);
	}
      else if (ip->data)
	{
	  char r[6];
	  r[0] = ' '; r[1] = 'r';
	  strcpy(r+2, ip->data+1);
	  fputs(r, fp);
	}
    }

  if (ip->glos)
    {
      if (ip->zoom && strcmp(ip->zoom, "entry_ids"))
	fputs(" zoom", fp);
    }
  else
    {
      if (strcmp(ip->zoom, "0"))
	fputs(" zoom", fp);	
    }

  fputc(' ', fp);
  if (ip->dors && '1' == *ip->dors)
    fputs("special", fp);
  else
    fputs("default", fp);

  if (ip->itemdata.proj && strcmp(ip->itemdata.proj, ip->project))
    fputs(" prox", fp);
  
}

void
pui_at_pager_data(Isp *ip, FILE *fp)
{
#define pattrs(a,v) fprintf(fp, " %s=\"%s\"", (a), (v))
#define pattrd(a,v) fprintf(fp, " %s=\"%d\"", (a), (v))
  pattrs("data-proj", ip->project);
  if (ip->glos)
    {
      pattrs("data-glos", ip->glos);
      pattrs("data-glet", ip->glosdata.let ? ip->glosdata.let : "entry_ids");
      if (ip->glosdata.ent)
	pattrs("data-gent", ip->glosdata.ent);
      if (ip->glosdata.xis)
	{
	  pattrs("data-gxis", ip->glosdata.xis);
	  pattrs("data-zoom", ip->zoom);
	  pattrs("data-sort", ip->perm);
	}
      else if (ip->srchdata.list)
	pattrs("data-srch", ip->srchdata.list);
    }
  else
    {
      if (ip->list_name) /* null for p4url; "list" when there's a srchdata.list */
	{
	  if (ip->srchdata.list)
	    pattrs("data-srch", ip->srchdata.list);
	  else
	    pattrs("data-list", ip->list_name);
	  pattrs("data-zoom", ip->zoom);
	  pattrs("data-sort", ip->perm);
	}
    }
  /* There can be both glos and item when item comes from a glossary
     instance set */
  if (ip->item)
    {
      const char *prev = ip->itemdata.prev ? ip->itemdata.prev : "";
      const char *next = ip->itemdata.next ? ip->itemdata.next : "";
      pattrs("data-prev", prev);
      pattrs("data-item", ip->item);
      pattrs("data-next", next);
      pattrs("data-bkmk", ip->item);
      if (ip->lang)
	pattrs("data-lang", ip->lang);
    }
  else if (ip->bkmk)
    pattrs("data-bkmk", ip->bkmk);

  if (ip->srch)
    pattrs("data-qury", ip->srch);

  if (ip->cemd && (!strcmp(ip->cemd, "kwic") || !strcmp(ip->cemd, "unit")))
    pattrs("data-cemd", ip->cemd);

  if (ip->itemdata.hili)
    pattrs("data-hili", ip->itemdata.hili);
}


void
pui_at_hili(Isp *ip, FILE *fp)
{
  if (ip->itemdata.hili)
    {
      fputs(" onload=\"p4Onload()\"", fp);
    }
}

void
pui_at_ceid(Isp *ip, FILE *fp)
{
  fputs(ip->ceid, fp);
}

void
pui_at_translations(Isp *ip, FILE *fp)
{
}

void
pui_at_current_page(Isp *ip, FILE *fp)
{
  fputs(ip->page, fp);
}

void
pui_at_active_pages(Isp *ip, FILE *fp)
{
  int npages = active_pages(ip);
  fputs(itoa(npages), fp);  
}

void
pui_at_page_label(Isp *ip, FILE *fp)
{
  fputs(active_pages(ip) == 1 ? "page" : "pages", fp);
}

void
pui_at_active_items(Isp *ip, FILE *fp)
{
  if (ip->glos && !ip->glosdata.xis)
    {
      if (ip->glosdata.let && strcmp(ip->glosdata.let, "entry_ids"))
	fputs(ip->glosdata.lmax, fp);
      else
	fputs(ip->glosdata.emax, fp);
    }
  else if (ip->itemdata.tmax && !strcmp(ip->zoom, "0"))
    fputs(ip->itemdata.tmax, fp);
  else
    fputs(itoa(ip->zmax), fp);
}

void
pui_at_item_label(Isp *ip, FILE *fp)
{
  char *label = "items";
  if (ip->glos && !ip->glosdata.xis)
    {
      int n = 0;
      if (ip->glosdata.emax)
	n = atoi(ip->glosdata.emax);
      else if (ip->glosdata.lmax)
	n = atoi(ip->glosdata.lmax);
      if (n == 1)
	label = "entry";
      else
	label = "entries";
    }
  else
    {
      if (ip->glosdata.xis)
	{
	  if (ip->itemdata.tmax)
	    {
	      if (!strcmp(ip->itemdata.tmax, "1"))
		label = "text";
	      else
		label = "texts";
	    }
	  else
	    {
	      if (ip->zmax == 1)
		label = "ref";
	      else
		label = "refs";
	    }
	}
      else
	{
	  if (ip->zmax == 1)
	    label = "item";
	}
    }
  fputs(label, fp);
}

static void
pui_at_item_index_sub(Isp *ip, FILE *fp, int typ, int nth)
{
  if (ip->data &&
      (!strcmp(ip->data, "dtxt") || (!strcmp(ip->data, "dglo") && !ip->glosdata.ent)))
    {
      if (typ)
	{
	  fputs("TEXT", fp);
	  if (nth)
	    fputc(' ', fp);
	}
      if (nth && ip->itemdata.index)
	{
	  if (ip->zoom && strcmp(ip->zoom, "0"))
	    fputs(ip->itemdata.zindex, fp);
	  else
	    fputs(ip->itemdata.index, fp);
	}
    }
  else
    {
      if (typ)
	{
	  fputs(ip->glos ? "ENTRY" : "ITEM", fp);
	  if (nth)
	    fputc(' ', fp);
	}
      
      if (nth && ip->itemdata.index)
	{
	  const char *index = ip->zoom ? ip->itemdata.zindex : ip->itemdata.index;
	  fputs(index, fp);
	}
    }
}

void
pui_at_item_index(Isp *ip, FILE *fp)
{
  pui_at_item_index_sub(ip, fp, 1, 1);
}

void
pui_at_item_index_nth(Isp *ip, FILE *fp)
{
  pui_at_item_index_sub(ip, fp, 0, 1);
}

void
pui_at_item_index_typ(Isp *ip, FILE *fp)
{
  pui_at_item_index_sub(ip, fp, 1, 0);
}

void
pui_at_srch_results(Isp *ip, FILE *fp)
{
  fprintf(fp, "%ld", ip->srchdata.count);
}

void
pui_at_srch_results_matches(Isp *ip, FILE *fp)
{
  pui_at_srch_results(ip, fp);
  if (ip->srchdata.count == 1)
    fputs(" match", fp);
  else
    fputs(" matches", fp);
}

void
pui_at_tmax(Isp *ip, FILE *fp)
{
  if (ip->itemdata.tmax)
    fputs(ip->itemdata.tmax, fp);
}

void
pui_at_tmax_texts(Isp *ip, FILE *fp)
{
  pui_at_tmax(ip, fp);
  if (ip->itemdata.tmax && !strcmp(ip->itemdata.tmax, "1"))
    fputs(" Text", fp);
  else
    fputs(" Texts", fp);
}

void
pui_at_srchterm(Isp *ip, FILE *fp)
{
  if (ip->srch)
    fputs(ip->srch, fp);
}

void
pui_at_environment(Isp *ip, FILE *fp)
{
  isp_environment(fp, ip);  
}

void
pui_at_error_log(Isp *ip, FILE *fp)
{
  if (ip->cache.sub)
    {
      char errbuf[strlen(ip->cache.sub)+strlen("/err.log0")];
      sprintf(errbuf, "%s/err.log", ip->cache.sub);
      if (!access(errbuf, R_OK))
	{
	  fprintf(fp, "\n\n<h1>Additional information from error log:</h1>\n\n");
	  file_copy(errbuf, "-");
	  fprintf(fp, "\n");
	}
    }
}

void
pui_at_error(Isp *ip, FILE *fp)
{
  fprintf(fp, ip->errx ? ip->err : PX_ERROR_START "%s", ip->errx ? ip->errx : ip->err);
  pui_at_error_log(ip, fp);
}

void
pui_at_invocation(Isp *ip, FILE *fp)
{
  isp_argv(fp, ip);
}

void
pui_at_status(Isp *ip, FILE *fp)
{
  isp_show(fp, ip);
}

void
pui_at_title(Isp *ip, FILE *fp)
{
  fprintf(fp, "%s &#x2014; %s", xpd_option(ip->xpd, "logo"), xpd_option(ip->xpd, "name"));
}

void
pui_at_project(Isp *ip, FILE *fp)
{
  if (ip->project)
    fputs(ip->project,fp);
  else
    fputs("[no project]",fp);
}

void
pui_at_generator(Isp *ip, FILE *fp)
{
  if (ip->pui)
    fprintf(fp, "oracc/px %s ", ip->pui);
  else
    fputs("oracc/px unknown", fp);
}

void
pui_at_item_qpqx(Isp *ip, FILE *fp)
{
  fprintf(fp, "%s:%s", ip->itemdata.proj, ip->itemdata.item);
}

void
pui_at_menu(Isp *ip, FILE *fp)
{
  if (!ip->srch || ip->srchdata.count != 0L)
    {
      if (ip->glos || !ip->curr_cfg || ip->curr_cfg->leftmenu)
	{
	  fprintf(fp, "<div id=\"p4Menu\">\n");
	  if (!ip->glos && ip->curr_cfg && ip->curr_cfg->select)
	    pui_at_select_sort(ip, fp);
	  px_file_copy(ip, ip->cache.zout, "-");
	  fprintf(fp, "</div>\n");
	}
      else
	fprintf(fp, "<div id=\"p4Menu\" class=\"hide none\"><p>x</p></div>\n");
    }
  /* can do better here? maybe return 123/0/1 menu for outlined.lst? */
}

void
pui_at_content(Isp *ip, FILE *fp)
{
  if (ip->item/* && !strcmp(ip->what, "text")*/)
    {
      if (ip->glos && !ip->glosdata.xis)
	{
	  if (pui_output(ip, stdout, pui_filetext("p4itemglo.xml")))
	    longjmp(ip->errjmp, 1);
	}
      else if (ip->nowhat)
	{
	  if (pui_output(ip, stdout, pui_filetext("p4nowhat.xml")))
	    longjmp(ip->errjmp, 1);
	}
      else
	{
	  const char *itemxtf = "p4itemxtf.xml";
	  if (ip->itemdata.not)
	    itemxtf = (ip->itemdata.not == 2 ? "p4unknown.xml" : "p4noxtf.xml");
	  if (pui_output(ip, stdout, pui_filetext(itemxtf)))
	    longjmp(ip->errjmp, 1);
	}
    }
  else if (ip->srch && ip->srchdata.count == 0L)
    {
      if (pui_output(ip, stdout, pui_filetext("p4nosrch.xml")))
	longjmp(ip->errjmp, 1);
    }
  else
    px_file_copy(ip, ip->cache.page, "-");
}

void
pui_at_item_meta(Isp *ip, FILE *fp)
{
  px_file_copy(ip, ip->cache.meta, "-");
}

void
pui_at_item_data(Isp *ip, FILE *fp)
{
  if (ip->glosdata.ipath) /* check this doesn't interact badly with ip->itemdata.html if there is an entry > instances > item */
    {
      fflush(fp);
      isp_glos_entry(ip);
    }
  else
    {
      if (ip->itemdata.hili)
	{
	  char *const *hvec = (char *const *)vec_from_str((char*)ip->itemdata.hili, NULL, NULL);
	  selecter(ip->itemdata.html, hvec);
	}	  
      else
	px_file_copy(ip, ip->itemdata.html, "-");
    }
}

void
pui_at_article(Isp *ip, FILE *fp)
{
  fflush(fp);
  isp_glos_entry(ip);
}

void
pui_at_select_ce(Isp *ip, FILE *fp)
{
  const char *params[] = { "select", "'line'", NULL };
  if (ip->cemd)
    {
      if (!strcmp(ip->cemd, "kwic"))
	params[1] = "'kwic'";
      else if (!strcmp(ip->cemd, "unit"))
	params[1] = "'unit'";
    }
  Xslt *xp = xslt_one_off("p4ceselect", p4ceselect, "p4select", p4select, "-", params);
  if (!xp)
    {
      ip->err = xslt_if_lasterr();
      longjmp(ip->errjmp, 1);
    }
  else
    xslt_term(xp);
}

  
void
pui_at_select_sort(Isp *ip, FILE *fp)
{
#if 1
  pui_123(ip, ip->perm, ip->curr_cfg->sort_labels, fp);
#else
  fputs("<div id=\"p4MenuSelect\">", fp);
  char perm[6]; sprintf(perm, "'%s'", ip->perm);
  const char *params[] = { "select", perm, NULL };
  char osbufd[strlen(ip->oracc)+strlen(ip->project)+strlen("//02xml/p4OSdefault.xml0")];
  sprintf(osbufd, "%s/%s/02xml/p4OSdefault.xml", ip->oracc, ip->project);
  Xslt *xp = xslt_one_off(osbufd, NULL, "p4select", p4select, "-", params);
  if (!xp)
    {
      ip->err = xslt_if_lasterr();
      longjmp(ip->errjmp, 1);
    }
  else
    xslt_term(xp);
  char osbufs[strlen(ip->oracc)+strlen(ip->project)+strlen("//02xml/p4OSspecial.xml0")];
  sprintf(osbufs, "%s/%s/02xml/p4OSspecial.xml", ip->oracc, ip->project);
  xp = xslt_one_off(osbufs, NULL, "p4select", p4select, "-", params);
  if (!xp)
    {
      ip->err = xslt_if_lasterr();
      longjmp(ip->errjmp, 1);
    }
  else
    xslt_term(xp);
  fputs("</div>", fp);
#endif
}

void
pui_at_select_trans(Isp *ip, FILE *fp)
{
#if 1
  if (ip->itemdata.langs)
    {
      fputs("<span>\n          <select id=\"p4TransSelect\" onchange=\"act_translation()\">\n", fp);
      int i;
      for (i = 0; ip->itemdata.langp[i]; ++i)
	{
	  if (ip->itemdata.xtflang && !strcmp(ip->itemdata.xtflang, ip->itemdata.langp[i]))
	    fprintf(fp, "            <option value=\"%s\" selected=\"selected\">%s</option>\n",
		    ip->itemdata.langp[i],ip->itemdata.langp[i]);
	  else
	    fprintf(fp, "            <option value=\"%s\">%s</option>\n",
		    ip->itemdata.langp[i],ip->itemdata.langp[i]);
	}
      fprintf(fp, "            <option value=\"none\">(none)</option>\n");
      fputs("          </select>\n        </span>\n", fp);
    }
#else
  const char *params[] = { "select", "'en'", NULL };
  char trbuf[strlen(ip->oracc)+strlen(ip->project)+strlen("//02xml/p4-trans-select.xml0")];
  sprintf(trbuf, "%s/%s/02xml/p4-trans-select.xml", ip->oracc, ip->project);
  Xslt *xp = xslt_one_off(trbuf, NULL, "p4select", p4select, "-", params);
  if (!xp)
    {
      ip->err = xslt_if_lasterr();
      longjmp(ip->errjmp, 1);
    }
  else
    xslt_term(xp);
#endif
}

void
pui_at_what_not(Isp *ip, FILE *fp)
{
  if (ip->what)
    {
      const char *w = ip->what;
      if (!strcmp(ip->what, "block"))
	w = "score block";
      else if (!strcmp(ip->what, "sources"))
	w = "sources table";
      fputs(w, fp);
    }
  else
    {
      fputs("[unknown requested data type]", fp);
    }
}

void
pui_at_cuneify_bar(Isp *ip, FILE *fp)
{
  if (ip->form && !strcmp(ip->form, "cuneify"))
    {
      fprintf(fp, "\t<div id=\"p4CuneifyBar\">"
	      "<p>Cuneified version of <a href=\"/%s/%s\" target=\"_new\">%s:%s</a> -- click on a button to switch fonts: "
	      "<span class=\"cfy-button cfy-noto\" onclick=\"cuneify_reset(event)\" data-font=\"noto\">NOTO</span>"
	      "<span class=\"cfy-button cfy-oobf\" onclick=\"cuneify_reset(event)\" data-font=\"oobf\">OOBF</span>"
	      "<span class=\"cfy-button cfy-nao\" onclick=\"cuneify_reset(event)\" data-font=\"nao\">NA-O</span>"
	      "</p></div>\n",
	      ip->itemdata.proj, ip->itemdata.item,
	      ip->itemdata.proj, ip->itemdata.item);
    }
}

void
pui_at_cuneify_onload(Isp *ip, FILE *fp)
{
  if (ip->form && !strcmp(ip->form, "cuneify"))
    fputs(" onload=\"p4Onload()\"", fp);
}

void
pui_at_item_img(Isp *ip, FILE *fp)
{
  const char *yes = hash_find(ip->xpd->opts, (ucp)"pager-item-images");
  if (yes && !strcmp(yes, "yes"))
    {
      if (!isp_item_img(ip))
	px_file_copy(ip, ip->itemdata.img, "-");
      else
	fputs("<div class=\"p4Images\"><p>(No image results)</p></div>", fp);
    }
}

void
pui_at_item_cfy(Isp *ip, FILE *fp)
{
  const char *yes = hash_find(ip->xpd->opts, (ucp)"pager-item-cuneify");
  if (yes && !strcmp(yes, "yes"))
    {
      if (!isp_item_cfy(ip))
	px_file_copy(ip, ip->itemdata.cfy, "-");
      else
	fputs("<div class=\"p4Cuneify\"><p>(No cuneify result)</p></div>", fp);
    }
}
