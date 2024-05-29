#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  if (!strcmp(perm, select))
    strcat(buf, "*");
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

struct s123 *
do123(const char *str, const char *select)
{
  static struct s123 s;
  int sizes[] = { 0, 2 , 3 , 7 };
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
      sprintf(s.sl[0].label+strlen(s.sl[0].label), "%s/%s", s.p[0], s.p[1]);
      
      s.sl[1].sort = "21";
      s.sl[1].label = malloc(s.len+1);
      if (!strcmp(s.sl[1].sort, select))
	strcpy(s.sl[1].label, "*");
      sprintf(s.sl[1].label+strlen(s.sl[1].label), "%s/%s", s.p[1], s.p[0]);
    }
  else
    {
      static char *s123[] = { "123" , "132" , "213" , "231" , "312" , "321" , NULL };
      int i;
      for (i = 0; s123[i]; ++i)
	{
	  s.sl[i].sort = s123[i];
	  s.sl[i].label = str123(&s, s123[i], select);
	}
    }

  return &s;
}

const char *single = "alpha";
const char *doublet = "semel,bis";
const char *triple = "one,two,three";

int
main(int argc, char **argv)
{
  struct s123 *sp = do123(triple, "231");
  int i;
  for (i = 0; sp->sl[i].sort; ++i)
    printf("%s -> %s\n", sp->sl[i].sort, sp->sl[i].label);
  putchar('\n');
  sp = do123(doublet, "21");
  for (i = 0; sp->sl[i].sort; ++i)
    printf("%s -> %s\n", sp->sl[i].sort, sp->sl[i].label);
  putchar('\n');
  sp = do123(single, NULL);
  for (i = 0; sp->sl[i].sort; ++i)
    printf("%s -> %s\n", sp->sl[i].sort, sp->sl[i].label);  
}
