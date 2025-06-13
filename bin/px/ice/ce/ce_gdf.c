#include <oraccsys.h>
#include <runexpat.h>
#include <gdf.h>
#include <xpd.h>
#include <xmlify.h>
#include "selib.h"
#include "ce.h"
#include "p2.h"

#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif

extern int p4;

extern int item_offset, tabbed;

extern const char *arg_fields, *gdf_xml, *idattr;
extern const char *gdfset, *mode, *project;
const char *gdf_fields = NULL, *gdf_widths = NULL;

extern const char **field_specs;
extern const char **width_specs;
extern List **field_lists;

static List *record_hashes;
static struct pool *record_pool;
static Hash *curr_record_hash = NULL;

static void gdfprinter2(Hash *fields);

extern int *idcountp;
extern Hash *ht;
extern int echoing;

void
gdfinit(void)
{
  record_hashes = list_create(LIST_SINGLE);
  record_pool = pool_init();
}

static int
alldigit(const char *s)
{
  while (isdigit(*s))
    ++s;
  return (*s == '\0');
}

void
gdf_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name,"o:record"))
    {
      const char *id = attr_by_name(atts,idattr);
      if (id && (idcountp = hash_find(ht,(const unsigned char*)id)))
	{
	  curr_record_hash = hash_create(1);
	  hash_add(curr_record_hash, (unsigned char *)"o:id", pool_copy((unsigned char *)id, record_pool));
	  echoing = 1;
	}
    }
  else if (echoing)
    {
      if (echoing++ > 1)
	fprintf(stderr, "ce_gdf: GDF records may not contain XML tags\n");
    }
  else
    charData_discard();
}

void
gdf_eH(void *userData, const char *name)
{
  if (echoing)
    {
      if (!strcmp(name,"o:record"))
	{
	  echoing = 0;
	  list_add(record_hashes, curr_record_hash);
	  curr_record_hash = NULL;
	  charData_discard();
	}
      else if (--echoing == 1)
	{
	  hash_add(curr_record_hash, 
		   pool_copy((unsigned char *)name, record_pool), 
		   pool_copy((unsigned char *)charData_retrieve(), record_pool));
	}
    }
  else
    {
      /* Support ID in o:record@xml:id or in o:record/o:id */
      if (!strcmp(name, "o:id"))
	{
	  const char *id = charData_retrieve();
	  if (id && (idcountp = hash_find(ht,(const unsigned char*)id)))
	    {
	      if (!curr_record_hash)
		{
		  curr_record_hash = hash_create(1);
		  hash_add(curr_record_hash,
			   (unsigned char *)"o:id", pool_copy((unsigned char *)id, record_pool));
		  echoing = 1;
		}
	    }	  
	}
      else 
	charData_discard();
    }
}

void
gdfprinter(void)
{
  list_exec(record_hashes, (void(*)(void*))gdfprinter2);
}

static void
gdfprinter2(Hash *fields)
{
  static int nth = 0;
  int i;
  const char *designation = NULL;
#if 0
  const char *icon = NULL, *icon_alt = NULL;
  const char *id = NULL;
#endif
  extern int in_group;

  if (!in_group)
    {
      in_group = 1;
      fputs("<ce:group>", stdout);
    }

  ++nth;

  const char *pqx = hash_find(fields, (ucp)"o:id");

  char anchor[strlen("<a href=.javascript://. onclick=.act_iref(event). data-iref=..0>")+strlen(pqx)];
  sprintf(anchor, "<a href=\"javascript://\" onclick=\"act_iref(event)\" data-iref=\"%s\">", pqx);
  fputs("<ce:data><tr xmlns=\"http://www.w3.org/1999/xhtml\">",stdout);

  fprintf(stdout,"<td class=\"ce-ood-id\">%s%s</a></td>", anchor, pqx);
  for (i = 0; width_specs[i]; ++i)
    {
      List *tmp = field_lists[i];
      const char *pct = width_specs[i];
      const char *field, *value, *field_used = "";
      char pctbuf[4];
      int this_is_designation = 0;

      for (field = list_first(tmp); field; field = list_next(tmp))
	{
	  if ((value = hash_find(fields, (unsigned char *)field)))
	    {
	      field_used = field;
	      break;
	    }
	}
      if (!strcmp(field_used, "n"))
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
	fprintf(stdout, "<td style=\"width: %s\"><a href=\"javascript:act_item_mode(%s,'xtf')\">%s</a></td>", pct, pqx, xmlify((unsigned char *)value));
      else
	fprintf(stdout, "<td style=\"width: %s;\">%s</td>", pct, xmlify((unsigned char *)value));
    }
  fputs("</tr></ce:data>",stdout);
}
