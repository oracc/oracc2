/* Unicode TR */
#include <oraccsys.h>
#include <dirent.h>

int istdin = 0;
int ostdout = 0;
int status = 0;
int verbose = 1;

unsigned long tmin = 0xfffffff;
unsigned long tmax = 0L;
wchar_t *trtab;

void
load_tab(char *tab)
{
  size_t ntab;
  uchar **lp = loadfile_lines((uccp)tab, &ntab);
  if (lp)
    {
      wchar_t wc[ntab];
      wchar_t to[ntab];
      int i;
      for (i = 0; i < ntab; ++i)
	{
	  char *e;
	  wc[i] = strtoul((ccp)lp[i],&e,16);
	  while (isspace(*e))
	    ++e;
	  to[i] = strtoul((ccp)e,NULL,16);
	  if (verbose > 2)
	    fprintf(stderr, "%s => %X\t%X\n", lp[i], wc[i], to[i]);
	  if (wc[i] >= 0x12690)
	    {
	      if (wc[i] > tmax)
		tmax = wc[i];
	      if (wc[i] < tmin)
		tmin = wc[i];
	    }
	}
      size_t tablen = tmax; /* - tmin */
      if (verbose > 1)
	{
	  fprintf(stderr, "tab %s has tmin=%lu and tmax=%lu\n", tab, tmin, tmax);
	  fprintf(stderr, "allocating tablen wchar_t = %lu bytes\n", tablen * sizeof(wchar_t));
	}
      trtab = calloc(tablen, sizeof(wchar_t));
      for (i = 0; i < ntab; ++i)
	trtab[wc[i]] = to[i];
      if (verbose > 2)
	{
	  for (i = 0; i < tablen; ++i)
	    {
	      if (trtab[i])
		fprintf(stderr, "%lX\t%lX\n", (unsigned long)i, (unsigned long)trtab[i]);
	    }
	}
    }
}

void
utr_file(char *fn)
{
  if (verbose)
    fprintf(stderr, "utr processing file: %s\n", fn);
  size_t n;
  char *f = NULL;
  
  if (!strcmp(fn, "-"))
    f = (char*)loadstdin(&n);
  else
    f = (char*)loadfile((uccp)fn, &n);

  if (f)
    {
      wchar_t wf[n+1];
      size_t wf_res = mbstowcs(wf, f, n);
      if (wf_res < 0)
	{
	  fprintf(stderr, "utr: error converting %s to wcs\n", fn);
	  perror("utr: system error");
	  ++status;
	  return;
	}
      int i;
      for (i = 0; wf[i]; ++i)
	{
	  if (wf[i] >= tmin && wf[i] <= tmax && trtab[wf[i]])
	    wf[i] = trtab[wf[i]];
	}
      free(f);
      int m = 6*wf_res;
      f = malloc(m+1);
      size_t f_res = wcstombs(f,wf,m+1);
      if (f_res < 0)
	{
	  fprintf(stderr, "utr: error converting %s back to mbs\n", fn);
	  perror("utr: system error: ");
	  ++status;
	}
      else
	{
	  int o = ostdout ? fileno(stdout) : open(fn, O_WRONLY);
	  write(o,f,f_res);
	  close(o);
	}
    }
}

void
utr_dir(const char *path)
{
  DIR *dir = opendir(path);
  if (dir == NULL)
    {
      perror("opendir");
      return;
    }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
	continue;
      }
      
      char full_path[1024];
      snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
      
      struct stat statbuf;
      if (stat(full_path, &statbuf) == 0)
	{
	  if (S_ISDIR(statbuf.st_mode))
	    {
	      if (verbose)
		fprintf(stderr, "utr entering directory: %s\n", full_path);
	      utr_dir(full_path);
	    }
	  else
	    {
	      /*printf("File: %s\n", full_path);*/
	      utr_file(full_path);
	    }
        }
    }
  closedir(dir);
}

int
main(int argc, char **argv)
{
  char *l = setlocale(LC_ALL, ORACC_LOCALE);
  int optix = 1;
  if (!strcmp(argv[optix], "-s"))
    {
      istdin = ostdout = 1;
      ++optix;
    }
  
  if (l)
    {
      if (verbose)
	fprintf(stderr, "utr set locale to %s\n", l);
      
      char *tabfn = argv[optix++];
      load_tab(tabfn);
      if (istdin)
	{
	  utr_file("-");
	}
      else
	{
	  char *start = argv[optix++];
	  struct stat statbuf;      
	  if (stat(start, &statbuf) == 0)
	    {
	      if (S_ISDIR(statbuf.st_mode))
		{
		  if (verbose)
		    fprintf(stderr, "utr start directory: %s\n", start);
		  utr_dir(start);
		}
	      else
		{
		  ostdout = 1;
		  utr_file(start);
		}
	    }
	}
    }
  else
    {
      fprintf(stderr, "utr failed to set locale to %s\n", ORACC_LOCALE);
      exit(1);
    }
}
