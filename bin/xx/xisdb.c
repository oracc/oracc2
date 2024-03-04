#include <oraccsys.h>
#include <dbi.h>

char *tis_file = NULL;
FILE *tis_fp = NULL;
  
struct tis_dir_name { const char *dir; const char *nam; };
struct tis_dir_name f_info = { NULL, NULL };
struct tis_info { long int seek; int len; int count; };
struct tis_info tis_bad = { -1, -1 };
struct tis_info t_info = { -1, -1 , -1};

Dbi_index *tis_dip = NULL;

void
tis_add(const char *r_id, long int s, int n, int l)
{
  t_info.seek = s;
  t_info.len = l;
  t_info.count = n + 1;
  if (DBI_BALK == dbi_add(tis_dip, (Uchar*)r_id, &t_info, 1))
    fprintf(stderr, "xisdb: will not add duplicate ID %s\n", r_id);
  l = 0;
}

void
tis_line()
{
  int ch, r_len = 0;
  int s = 0, n = 0, l = 0;
#define R_LEN 127
  static char r_buf[R_LEN+1];
  static int tis_line = 0;

  ++tis_line;
  while (EOF != (ch = fgetc(tis_fp)))
    {  
      if ('\n' == ch)
	break;
      else if ('\t' == ch)
	{
	  if (r_len >= 0)
	    {
	      r_buf[r_len] = '\0';
	      r_len = -1;
	    }
	  s = ftell(tis_fp);
	  n = l = 0;
	}
      else if (' ' == ch)
	{
	  ++n;
	  ++l;
	}
      else
	{
	  if (r_len >= 0)
	    {
	      if (r_len < R_LEN)
		r_buf[r_len++] = ch;
	      else
		{
		  fprintf(stderr, "%s:%d: invalid ID while creating xisdb. Stop\n",
			  tis_file, tis_line);
		  exit(1);
		}
	    }
	  else
	    ++l;
	}
    }
  if (l > 0)
    tis_add(r_buf, s, n, l);
}

void
tis_index(const char *dir, const char *name, const char *tis_file)
{
  if ((tis_fp = fopen(tis_file, "r")))
    {
      if ((tis_dip = dbi_create (name, dir, 1024, sizeof(struct tis_info), DBI_BALK)))
	{
	  do
	    tis_line();
	  while (!feof(tis_fp));
	  dbi_flush(tis_dip);
	}
      else
	{
	  fclose (tis_fp);
	  fprintf(stderr, "xisdb: failed to create %s/%s.dbh\n", dir, name);
	  exit(1);
	}
    }
  else
    {
      fprintf(stderr, "xisdb: failed to open %s\n", tis_file);
      exit(1);
    }

}

void
tis_init(const char *dir, const char *name, const char *tis_file)
{
  tis_dip = dbi_open(name, dir);
}

int
tis_exists(const char *id)
{
  dbi_find(tis_dip, (Uchar*)id);
  return tis_dip->nfound;
}

struct tis_info
tis_find(const char *id)
{
  struct tis_info *tip = NULL;
  dbi_find (tis_dip, (Uchar*)id);
  if (tis_dip->nfound)
    {
      tip = dbi_detach_data(tis_dip, NULL);
      return *tip;
    }
  else
    return tis_bad;
}

void
tis_term ()
{
  if (tis_dip)
    {
      dbi_close(tis_dip);
      tis_dip = NULL;
    }
}

void
tis_paths(const char *argfile)
{
  char *tmp;
  f_info.nam = "tis";
  tis_file = malloc(strlen(argfile)+1);
  strcpy(tis_file, argfile);
  if ((tmp = strrchr(tis_file, '.')))
    {
      *tmp = '\0';
      if ((tmp = strrchr(tis_file, '/')))
	{
	  *tmp++ = '\0';
	  f_info.dir = tis_file;
	}
      else
	{
	  f_info.dir = "";
	}
    }
}

void
tis_dump(const char *tis_file)
{
  FILE *tis_fp = NULL;
  if ((tis_fp = fopen(tis_file, "r")))
    {
      int n = 0;
      fseek(tis_fp, t_info.seek, 0);
      while (n++ <= t_info.len)
	{
	  int ch = fgetc(tis_fp);
	  if (ch == EOF)
	    {
	      fprintf(stderr, "xisdb: %s, seek=%ld, len=%d: ran out of bytes\n",
		      tis_file, t_info.seek, t_info.len);
	      exit(1);
	    }
	  else
	    {
	      if (ch == ' ')
		putchar('\n');
	      else
		putchar(ch);
	    }
	}
      fclose(tis_fp);
    }
}

int
main(int argc, char **argv)
{
  int idarg = 2;
  int count_only = 0, exists_only = 0;
  if (argv[1])
    {
      if (!access(argv[1], R_OK))
	{
	  tis_paths(argv[1]);
	  if (argv[2] && !strcmp(argv[2], "-x"))
	    {
	      idarg = 3;
	      exists_only = 1;
	    }
	  else if (argv[2] && !strcmp(argv[2], "-c"))
	    {
	      idarg = 3;
	      count_only = 1;
	    }
	  if (argv[idarg])
	    {
	      {
		tis_init(f_info.dir, f_info.nam, argv[1]);
		if (exists_only)
		  {
		    exit(!tis_exists(argv[idarg]));
		  }
		else
		  {
		    t_info = tis_find(argv[idarg]);
		    if (count_only)
		      printf("%d", t_info.count);
		    else
		      tis_dump(argv[1]);
		  }
		tis_term();
	      }
	    }
	  else
	    tis_index(f_info.dir, f_info.nam, argv[1]);
	  if (tis_file)
	    free(tis_file);
	}
      else
	fprintf(stderr, "xisdb: %s non-existent or unreadable\n", argv[1]);
    }
  else
    {
      fprintf(stderr, "xisdb: usage: xisdb file.tis [ID]\n");
      exit(1);
    }

  return 0;
}

int verbose = 0;
const char *prog = "xisdb";
int major_version = 1;
int minor_version = 0;
const char *usage_string = " TIS_FILE [-x] [XML_ID]";
int
opts(int argc, const char *arg) { return 0;}
void help() {}
