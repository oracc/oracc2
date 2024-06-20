#include <oraccsys.h>
#include <dbxlib.h>
#include <xmlify.h>
#include <xmd.h>
#include <xpd.h>
#include "ce.h"
#include "p2.h"

#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif

static int nfields = 0, nwidths = 0, nlabels = 0;

Dbi_index *prxdp;

extern int p4;

extern int item_offset, tabbed;
extern const char *arg_fields;
extern const char *mode, *project;
const char *xmd_fields = NULL, *xmd_widths = NULL, *xmd_labels;

static const char *default_fields 
  = "designation,primary_publication,subgenre|genre,period,place|provenience";
static const char *default_widths = "auto,17,17,17,17";
static const char *default_labels = NULL; /*"Designation,Publication,Content,Period,Provenience";*/

static Pool *ce_xmd_pool;

const char **field_specs;
const char **width_specs;
const char **label_specs;
List **field_lists;

int
xmd_field_count(void)
{
  return nfields;
}

int
count_entries(const char *tmp, const char *option)
{
  int i;

  if (!tmp)
    return 0;

  for (i = 1; *tmp; ++tmp)
    {
      if (',' == *tmp)
	{
	  ++i;
	  ++tmp;
	  while (isspace(*tmp))
	    ++tmp;
	  if (',' == *tmp)
	    {
	      fprintf(stderr, "ce_xmd2: %s/00lib/config.xml: empty field in `%s'\n",
		      project, option);
	      ++tmp;
	      while (isspace(*tmp) || ',' == *tmp)
		++tmp;
	    }
	}
    }
  return i;
}

void
set_entries(const char **entries, const char *option)
{
  int i;
  char *tmp;

  if (!option)
    return;

  if (!state)
    state = "default";

  tmp = malloc(strlen(option)+1);
  (void)strcpy(tmp, option);
  for (i = 0, entries[0] = tmp; *tmp; )
    {
      if (',' == *tmp)
	{
	  *tmp++ = '\0';
	  while (isspace(*tmp))
	    ++tmp;
	  if (',' == *tmp)
	    {
	      fprintf(stderr, "ce_xmd2: %s/00lib/config.xml: empty field in `%s'\n",
		      project, option);
	      ++tmp;
	      while (isspace(*tmp) || ',' == tmp[1])
		++tmp;
	    }
	  if (*tmp)
	    entries[++i] = tmp;
	}
      else
	++tmp;
    }
  entries[++i] = NULL;
}

static void
set_field_lists(const char **fieldspecs)
{
  int i;
  extern const char *list_vec_sep_str;
  list_vec_sep_str = "|";
  field_lists = malloc((1+nfields) * sizeof(List *));
  for (i = 0; fieldspecs[i]; ++i)
    {
      char *str = malloc(strlen(fieldspecs[i])+1);
      strcpy(str, fieldspecs[i]);
      field_lists[i] = list_from_str(str, NULL, LIST_SINGLE);
      free(str);
    }
  field_lists[i] = 0;
}

int
xmdinit2(const char *project)
{
  struct p2_options *p2opt = NULL;

  ce_xmd_pool = pool_init();
  p2opt = p2_load(project, state, ce_xmd_pool);

  if (!xmd_fields)
    xmd_fields = (p2opt->catalog_fields ? p2opt->catalog_fields : default_fields);
  if (!xmd_widths)
    xmd_widths = (p2opt->catalog_widths ? p2opt->catalog_widths : default_widths);
  if (!xmd_labels)
    xmd_labels = (p2opt->catalog_labels ? p2opt->catalog_labels : default_labels);

  nfields = count_entries(xmd_fields, "catalog-fields");
  nwidths = count_entries(xmd_widths, "catalog-widths");
  nlabels = count_entries(xmd_labels, "catalog-widths");

  if (nfields != nwidths)
    {
      fprintf(stderr, 
	      "ce_xmd2: %s/00lib/config.xml: `%s-catalog-fields' and `%s-catalog-widths' should have same number of entries\n",
	      project, state, state);
      return 1;
    }
  if (xmd_labels && nfields != nlabels)
    {
      fprintf(stderr, 
	      "ce_xmd2: %s/00lib/config.xml: `%s-catalog-fields' and `%s-catalog-labels' should have same number of entries\n",
	      project, state, state);
      return 1;
    }
  

  field_specs = malloc((nfields+1)*sizeof(const char *));
  width_specs = malloc((nwidths+1)*sizeof(const char *));
  label_specs = malloc((nlabels+1)*sizeof(const char *));

  set_entries(field_specs, xmd_fields);
  set_entries(width_specs, xmd_widths);
  if (xmd_labels)
    set_entries(label_specs, xmd_labels);

  set_field_lists(field_specs);

  return 0;
}

static int
alldigit(const char *s)
{
  while (isdigit(*s))
    ++s;
  return (*s == '\0');
}

/* Previous version had this:

   Hack designation/primary_publication issues: if 0 is empty, use 1;
     if 0 == 1, empty 1 
  if (!strcmp(print_ptrs[0]," "))
    {
      print_ptrs[0] = print_ptrs[1];
      print_ptrs[1] = " ";
    }
  else if (!strcmp(print_ptrs[0], print_ptrs[1]))
    print_ptrs[1] = " ";
 */
void
xmdprinter2(const char *pq)
{
  static int nth = 0;
  extern int in_group;

  if ('#' == *pq)
    {
      if (in_group)
	fputs("</ce:group>", stdout);
      else
	in_group = 1;
      fputs("<ce:group>", stdout);
      fprintf(stdout, "<ce:heading>%s</ce:heading>", ce_h(++pq));
    }
  else
    {
      Hash *fields = NULL;
      int i;
      const char *designation = NULL;
      /*const char *icon = NULL, *icon_alt;*/

      ++nth;
      xmd_init();

#if 1
      const char *catproj = NULL;
      const char *textid = NULL;
      const char *textcat = NULL;
      char xpqx[strlen(pq)+1];      
      strcpy(xpqx, pq);
      char *colon = strchr(xpqx, ':');
      if (colon)
	{
	  textcat = xpqx;
	  *colon = '\0';
	  textid = colon+1;
	}
      else
	textid = xpqx;
      char *at = strchr(xpqx, '@');
      if (at)
	*at++ = '\0';     
      if (prxdp)
	catproj = dbx_key(prxdp, textid, NULL);
      if (!catproj)
	{
	  if (at)
	    catproj = at;
	  else if (textcat)
	    catproj = textcat;
	  else
	    catproj = project;
	}
      fields = xmd_load(catproj, textid);
#else
      /* pq is a qualified ID, so use the project from that */
      char *at;
      if ((at = strchr(pq, '@')))
	*at = '\0';
      if ((pqx = strchr(pq, ':')))
	{
	  ++pqx;
	  const char *fn = expand(project,pqx,"xmd");
	  /* only use the qualified project if there is no local XMD */
	  if (!xaccess(fn,R_OK,0))
	    fields = xmd_load(project, pqx);
	  else 
	    fields = xmd_load(NULL, pq);
	}
      else
	{
	  fields = xmd_load(project, pq);
	  pqx = pq;
	}
#endif
      char anchor[strlen("<a href=.javascript://..onclick=.act_iref(event)..data-iref=.P123456.0")];
      sprintf(anchor, "<a href=\"javascript://\" onclick=\"act_iref(event)\" data-iref=\"%s\">", textid);
      
      if (!in_group)
	{
	  in_group = 1;
	  fputs("<ce:group>", stdout);
	}

      fputs("<ce:data><tr xmlns=\"http://www.w3.org/1999/xhtml\">",stdout);

      fprintf(stdout,
	      "<td class=\"ce-xmd-icon\">%s<img src=\"/img/viewtext.png\" alt=\"View text\"/></a></td>",
	      anchor);
      for (i = 0; width_specs[i]; ++i)
	{
	  List *tmp = field_lists[i];
	  const char *pct = width_specs[i];
	  const char *field, *value, *field_used = "";
	  char pctbuf[4];
	  int this_is_designation = 0;

	  for (field = list_first(tmp); field; field = list_next(tmp))
	    {
	      if ((value = hash_find(fields, (unsigned char *)field)) && strlen(value))
		{
		  field_used = field;
		  break;
		}
	    }
	  if (!strcmp(field_used, "designation") || !strcmp(field_used, "title"))
	    {
	      designation = value;
	      this_is_designation = 1;
	    }
	  if (!value || !strlen(value)
	      || (designation 
		  && !strcmp(field_used, "primary_publication")
		  && !strcmp(designation, value)))
	    value = " ";
	  if (alldigit(pct))
	    {
	      strncpy(pctbuf,pct,2);
	      pctbuf[2] = '%';
	      pctbuf[3] = '\0';
	      pct = pctbuf;
	    }
	  if (this_is_designation || i < link_fields)
	    fprintf(stdout, "<td style=\"width: %s\">%s%s</a></td>", pct, anchor, xmlify((unsigned char *)value));
	  else
	    fprintf(stdout, "<td style=\"width: %s;\">%s</td>", pct, xmlify((unsigned char *)value));
	}
      fputs("</tr></ce:data>",stdout);
    }
}

void
xmd_print_labels(void)
{
  if (nlabels)
    {
      int i;
      fputs("<ce:labels><thead><tr>",stdout);
      if (ood_mode)
	fputs("<th>ID</th>",stdout);
      else
	fputs("<th> </th>",stdout);
      for (i = 0; label_specs[i]; ++i)
	fprintf(stdout, "<th>%s</th>", label_specs[i]);
      fputs("</tr></thead></ce:labels>",stdout);
    }
}
