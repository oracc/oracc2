#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../isp/isp.h"

extern const char *s123[];

struct s123sl
{
  const char *sort;
  char *label;
};
  
struct s123
{
  int lev;
  int len;
  char *s;
  char *p[3];
  struct s123sl *sl;
};

static char *
str123(struct s123 *sp, const char *perm, const char *select)
{
  char *buf = malloc(sp->len+1);
  *buf = '\0';
  const char *p = perm;
  while (*p)
    {
      if (*p >= '1' && *p <= '3')
	{
	  if (sp->p[*p-'1'])
	    {
	      if (*buf)
		strcat(buf, "/");
	      strcat(buf, sp->p[*p-'1']);
	    }
	}
      ++p;
    }
  return buf;
}

static struct s123 *
do123(Isp *ip, const char *str, const char *select)
{
  static struct s123 s;
  int sizes[] = { 0, 2 , 3 , 7 };

  /* ip->perm is initialized to '#'; this ensures that it is reset to
     the default if it hasn't been reset earlier */
  if (!str || '#' == *str)
    {
      str = isp_set_perm(select);
      ip->perm = str;
    }
  
  s.len = strlen(str)+1;
  s.s = strdup(str);
  char *t = s.s;
  s.p[0] = s.s;
  if ((t = strchr(s.s, ',')))
    {
      *t++ = '\0';
      s.p[1] = t;
      if ((t = strchr(s.p[1], ',')))
	{
	  *t++ = '\0';
	  s.p[2] = t;
	}
      else
	s.p[2] = NULL;
    }
  else
    s.p[1] = NULL;

  if (s.p[2])
    s.lev = 3;
  else if (s.p[1])
    s.lev = 2;
  else
    s.lev = 1;

  s.sl = calloc(sizes[s.lev], sizeof(struct s123sl));

  if (s.lev == 1)
    {
      s.sl[0].sort = "1";
      s.sl[0].label = malloc(s.len+1);
      sprintf(s.sl[0].label, "*%s", s.p[0]);
    }
  else if (s.lev == 2)
    {
      s.sl[0].sort = "12";
      s.sl[0].label = malloc(s.len+1);
      if (!strcmp(s.sl[0].sort, select))
	strcpy(s.sl[0].label, "*");
      sprintf(s.sl[0].label, "%s/%s", s.p[0], s.p[1]);
      
      s.sl[1].sort = "21";
      s.sl[1].label = malloc(s.len+1);
      sprintf(s.sl[1].label, "%s/%s", s.p[1], s.p[0]);
    }
  else
    {
      int i;
      for (i = 0; s123[i]; ++i)
	{
	  s.sl[i].sort = s123[i];
	  s.sl[i].label = str123(&s, s123[i], select);
	}
    }

  return &s;
}

#if 0
<select id="p4OSdefault" onchange="act_sorter_default()" value="123">
  <option value="312">Place/Time/Genre</option>
  <option value="321">Place/Genre/Time</option>
  <option value="132">Time/Place/Genre</option>
  <option value="123" selected="selected">Time/Genre/Place</option>
  <option value="231">Genre/Place/Time</option>
  <option value="213">Genre/Time/Place</option>
</select>
#endif
  
int
pui_123(Isp *ip, const char *perm, const char *cfg, FILE *fp)
{
  struct s123 *sp = do123(ip, cfg, perm);
  int i;
  fputs("<div id=\"p4MenuSorter\">\n", fp);
  fprintf(fp, "<select id=\"p4MenuSelect\" onchange=\"act_sorter()\" value=\"%s\">\n", perm);
  for (i = 0; sp->sl[i].sort; ++i)
    {
      const char *sel = "";
      if (!strcmp(sp->sl[i].sort, perm))
	sel = " selected=\"selected\"";
      fprintf(fp, "<option value=\"%s\"%s>%s</option>\n", sp->sl[i].sort, sel, sp->sl[i].label);
    }
  fputs("</select>\n</div>\n", fp);
  return 0;
}

#if 0
const char *single = "alpha";
const char *doublet = "semel,bis";
const char *triple = "one,two,three";

int
main(int argc, char **argv)
{
#if 1
  isp_123("231", triple, stdout);
#else
  sp = do123(doublet, "21");
  for (i = 0; sp->sl[i].sort; ++i)
    printf("%s -> %s\n", sp->sl[i].sort, sp->sl[i].label);
  putchar('\n');
  sp = do123(single, NULL);
  for (i = 0; sp->sl[i].sort; ++i)
    printf("%s -> %s\n", sp->sl[i].sort, sp->sl[i].label);
#endif
}

#endif
