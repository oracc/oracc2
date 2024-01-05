#include <oraccsys.h>
#include <tis.h>

Tisp tish;

int basecount = 5;
const char *field = "period";
const char *project = "pctc";
int simple_mode = 0;
int threshold = 30;

static char *
first_n(int n, char *s)
{
  int i = 0;
  char *start = s;
  while (*s)
    {
      while (*s && ' ' != *s)
	++s;
      if (++i == n)
	break;
      else if (*s)
	++s;
    }
  if (*s)
    *s = '\0';
  return start;
}

static void
putn(int n, const char *s)
{
  int i = 0;
  while (*s)
    {
      while (*s && ' ' != *s)
	{
	  putchar(*s);
	  ++s;
	}
      if (++i == n)
	break;
      else if (*s)
	{
	  putchar(' ');
	  ++s;
	}
    }
  putchar('\n');
}

void
tis_div(struct tis_data *tip)
{
  FILE *xp;
  char fn[strlen(tip->key) + 12];
  
  int all_mode = atoi(tip->cnt) <= threshold;
  struct tis_data *sip;
  if (simple_mode)
    printf("h1\t%s\t%s\t%s\n", tip->key, tip->ref, tip->cnt);
  else
    {
      sprintf(fn, "tisdiv.d/%s.div", tip->key);
      if (!(xp = fopen(fn, "w")))
	{
	  fprintf(stderr, "tisdiv: can't write %s. Stop.\n", fn);
	  exit(1);
	}
      fputs("<div class=\"tisdiv\">", xp);
      fprintf(xp, "<h1>%s occurrences in total <a href=\"/%s/%s.tis\">See all instances</a></h1>",
	      tip->cnt, project, tip->ref);
    }
  
  for (sip = list_first(tip->subs); sip; sip = list_next(tip->subs))
    {
      const char *f = strstr(sip->key, field);
      if (f && '|' == f[-1] && '=' == f[strlen(field)])
	{
	  f = strchr(f, '/'); ++f;
	  if (simple_mode)
	    {
	      printf("h2\t%s\t%s\t%s\n#", f, sip->ref, sip->cnt);
	      if (all_mode)
		puts(sip->iss);
	      else
		putn(basecount, sip->iss);
	    }
	  else
	    {
	      fprintf(xp, "<h2>%s occurrences in %s <a href=\"/%s/%s.tis\">: see all %s instances</a></h2>",
		      sip->cnt, f, project, sip->ref, f);
	      char *iss = NULL;
	      if (all_mode)
		iss = sip->iss;
	      else
		iss = first_n(basecount, sip->iss);
	      char syscmd[strlen(iss)+strlen(project)+strlen(oracc_builds())+32];
	      sprintf(syscmd, "%s/bin/tisdiv.sh %s %s >>%s", oracc_builds(), project, iss, fn);
	      system(syscmd);
	    }
	}
    }

  if (simple_mode)
    puts("h0");
  else
    {
      fputs("</div>", xp);
      fclose(xp);
    }
}

void
tis_divs(Tisp t)
{
  int nk;
  const char **tk = hash_keys2(t, &nk);
  qsort(tk,nk,sizeof(const char *), cmpstringp);
  for (nk = 0; tk[nk]; ++nk)
    tis_div(hash_find(t, (ucp)tk[nk]));
}

int
main(int argc, char **argv)
{
  FILE *tfp = fopen(argv[1], "r");
  tis_load_init();
  tish = tis_load(tfp);
  tis_divs(tish);
  tis_load_term(tish);
}
