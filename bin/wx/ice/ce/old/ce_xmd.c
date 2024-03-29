#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <psdtypes.h>
#include <hash.h>
#include <runexpat.h>
#include <xmlutil.h>
#include <loadfile.h>
#include <messages.h>
#include "ce.h"

#ifndef strdup
extern char *strdup(const char *);
#endif
#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif

int curr_field, nprint_ptrs;
extern int tabbed;
extern const char *arg_fields;
extern const char *mode;

static const char *default_fields 
  = "designation,primary_publication,subgenre|genre,period,place|provenience";
static const char *default_template = 
  "<tr><td class=\"pc25\">%s</td>"
  "<td class=\"pc15\">%s</td>"
  "<td class=\"pc15\">%s</td>"
  "<td class=\"pc10\">%s</td>"
  "<td class=\"pc10\">%s</td></tr>"
  ;
static const char *template = NULL;
static int **template_makers = NULL;
Hash_table *fields_hash;

char **field_ptrs = NULL;
char **print_ptrs = NULL;
char *field_pool = NULL, *field_pool_insertp;

static void
xmd_sH(void *userData, const char *name, const char **atts)
{
  charData_discard();
}

static void
xmd_eH(void *userData, const char *name)
{
  int *ip = hash_find(fields_hash,(const unsigned char *)name);
  progress("xmd_eh: name = %s\n", name);
  if (ip)
    {
      field_ptrs[*ip] = field_pool_insertp;
      strcpy(field_ptrs[*ip], charData_retrieve());
      field_pool_insertp += strlen(field_ptrs[*ip]) + 1;
      progress("field %d set to %s\n",*ip,field_ptrs[*ip] ? field_ptrs[*ip] : "");
    }
}

static void
xmd_set_fields(const char*fp)
{
  int template_entries = 1, nfields = 1;
  char *fp2 = strdup(fp);
  char *p;
  int i, j;
  int *hash_data;

  for (p = (char*)fp; *p; ++p)
    {
      if (',' == *p)
	{
	  ++template_entries;
	  ++nfields;
	}
      else if ('|' == *p)
	++nfields;
    }
  fields_hash = hash_create(nfields);
  template_makers = calloc(template_entries,sizeof(int*));
  for (p = fp2, curr_field = i = 0; *p; ++curr_field)
    {
      const char *field = p;
      char type;

      while (*p && *p != ',' && *p != '|')
	++p;
      if (*p)
	{
	  type = *p;
	  *p++ = '\0';
	}
      else
	type = ',';
      if (type == '|')
	{
	  if (!template_makers[i])
	    {
	      int nopt = 1;
	      char *p2 = p;
	      while (*p2 && *p2 != ',')
		{
		  if ('|' == *p2)
		    ++nopt;
		  ++p2;
		}
	      template_makers[i] = malloc((1+nopt) * sizeof(int));
	      j = 0;
	    }
	  template_makers[i][j] = curr_field;
	  hash_data = &template_makers[i][j++];
	}
      else
	{
	  if (!template_makers[i])
	    {
	      template_makers[i] = malloc(2 * sizeof(int));
	      template_makers[i][0] = curr_field;
	      template_makers[i][1] = -1;
	      hash_data = &template_makers[i][0];
	      j = 0;
	    }
	  else
	    {
	      template_makers[i][j] = curr_field;
	      hash_data = &template_makers[i][j++];
	      template_makers[i][j] = -1;
	    }
	  ++i;
	}
      hash_add(fields_hash,(unsigned char *)field,hash_data);
      progress("field %d is %s\n",*hash_data, field);
    }
  nprint_ptrs = template_entries;
}

void
xmdinit(const char *project)
{
  char config_fn[_MAX_PATH];
  struct stat sbuf;
  const char *fields = arg_fields;
  
  /* load project config or use default_fields/default_template */;
  sprintf(config_fn,"/home/oracc/www/%s/config",project);
  if (xaccess(config_fn,R_OK,0))
    sprintf(config_fn,"/home/oracc/www/%s/config.new",project);

  if (!stat(config_fn,&sbuf))
    {
      unsigned char **lines;
      size_t nlines;
      int i;
      int ok_mode = 1;
      
      lines = loadfile_lines((unsigned char *)config_fn,&nlines);
      for (i = 0; i < nlines; ++i)
	{
	  if (mode && !strncmp((const char *)lines[i],"mode ",5))
	    {
	      unsigned char *lp = lines[i];
	      lp += 5;
	      while (isspace(*lp))
		++lp;
	      if (!strcmp((const char *)lp, mode))
		ok_mode = 1;
	      else
		ok_mode = 0;
	    }
	  else if (mode && !strcmp((const char *)lines[i], "mode off"))
	    ok_mode = 1;
	  else if (ok_mode && !strncmp((const char*)lines[i],"ce_xmd_fields ",14))
	    {
	      fields = (const char*)&lines[i][14];
	      while (isspace(*fields))
		++fields;
	    }
	  else if (ok_mode && !strncmp((const char *)lines[i],"ce_xmd_template ",16))
	    {
	      template = (const char*)&lines[i][16];
	      while (isspace(*template))
		++template;
	    }
	}
    }
  if (!fields)
    fields = default_fields;
  if (!template)
    template = default_template;

  xmd_set_fields(fields);

  field_ptrs = malloc(curr_field * sizeof(char*));
  print_ptrs = malloc(nprint_ptrs * sizeof(char*));
  field_pool = malloc(curr_field * 1024);
  *field_pool = '\0';
}

void
xmdprinter(const char *f)
{
  int i, j;
  const char *f_arg[2] = { NULL, NULL };
  f_arg[0] = f;

  /*initialize strings array to 0's */
  memset(field_ptrs, '\0', curr_field * sizeof(char*));
  memset(print_ptrs, '\0', nprint_ptrs * sizeof(char*));
  *field_pool = '\0';
  field_pool_insertp = field_pool;

  /*load strings from xmd file */
  runexpat(i_list,f_arg,xmd_sH,xmd_eH);

  fputs("<ce:data>", stdout);
  /*set print args strings (may involve x|y processing)*/
  for (i = 0; i < nprint_ptrs; ++i)
    {
      print_ptrs[i] = " ";
      for (j = 0; template_makers[i][j] != -1; ++j)
	{
	  const char *str = field_ptrs[template_makers[i][j]];
	  if (str && strlen(str) && *str != ' ')
	    {
	      print_ptrs[i] = (char*)str;
	      break;
	    }
	}
    }

  /* Hack designation/primary_publication issues: if 0 is empty, use 1;
     if 0 == 1, empty 1 */
  if (!strcmp(print_ptrs[0]," "))
    {
      print_ptrs[0] = print_ptrs[1];
      print_ptrs[1] = " ";
    }
  else if (!strcmp(print_ptrs[0], print_ptrs[1]))
    print_ptrs[1] = " ";

  if (tabbed)
    {
      int i;
      for (i = 0; i < nprint_ptrs; ++i)
	{
	  fputs(print_ptrs[i],stdout);
	  if ((i + 1) < nprint_ptrs)
	    putchar('\t');
	}
      putchar('\n');
    }
  else
    {
#if 0
  /* This worked on 32 bit GCC 3, but bombs on 64 bit GCC 4, 
     so I wrote a simple substitute ... */
  vfprintf(stdout,template,print_ptrs);
#else
      const char *tp;
      int i;
      for (tp = template, i = 0; *tp && i < nprint_ptrs; ++i)
	{
	  while (*tp && (tp[0] != '%' || tp[1] == '%'))
	    fputc(*tp++,stdout);
	  if (*tp)
	    {
	      if (print_ptrs[i])
		fputs(xmlify(print_ptrs[i]), stdout);
	      else
		fputs("ERROR: RAN OUT OF POINTERS", stdout);
	      tp += 2; /* just assume it is %s, which it is supposed to be ... */
	    }
	}
#endif
      if (xml_output > 1) /* S2 */
	{
	  fputs("</td></tr>", stdout);
	  fputs("</ce:data>", stdout);
	}
      else
	{
	  putchar('\n');
	  putchar(0);
	}
    }
}
