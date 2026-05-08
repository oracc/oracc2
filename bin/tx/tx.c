#include <oraccsys.h>
#include <tsv.h>

int verbose = 0;
const char *prog = "tx";
int major_version = 1;
int minor_version = 0;
const char *usage_string = " [-t tsv] [-d dir] [-n name] [-k key]";
  
const char *arg_dir, *arg_key, *arg_name, *arg_tsv;

const char *keys_file;

int ood_id, ood_pad, space_newlines, undbi;

int
main(int argc, char **argv)
{
  program_values(prog, major_version, minor_version, usage_string, NULL);
  options(argc, argv, "d:fk:K::n:o:st:u");
  const char *k1[2] = { arg_key , NULL };

  if (keys_file || arg_key)
    {
      const char **kk = k1;
      size_t n = 1;
      int res;
      if (keys_file)
	kk = (const char **)loadfile_lines3((uccp)keys_file, &n, NULL);
      res = tsv_lookup(arg_tsv, arg_dir, arg_name, kk, n);
      exit(res);
    }
  else if (undbi)
    {
      int res = tsv_undbi(arg_tsv, arg_dir, arg_name);
      exit(res);
    }
  else if (arg_tsv)
    {
      /* index tsv */
      int res = tsv_index(arg_tsv, arg_dir, arg_name);
      exit(res);
    }
  else if (ood_pad)
    {
      char *lp;
      switch (ood_pad)
	{
	case 5:
	  while ((lp = (char*)loadoneline(stdin,NULL)))
	    {
	      printf("%05d\t", ++ood_id);
	      puts(lp);
	    }
	  break;
	default:
	  break;
	}
    }
  else
    usage();

  return 0;
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'd':
      arg_dir = arg;
      break;
    case 'f':
      full_record = 1;
      break;
    case 'K':
      if (arg)
	keys_file = arg;
      else
	keys_file = "-";
      break;
    case 'k':
      arg_key = arg;
      break;
    case 'n':
      arg_name = arg;
      break;
    case 'o':
      ood_pad = atoi(arg);
      break;
    case 's':
      space_newlines = 1;
      break;
    case 't':
      arg_tsv = arg;
      break;
    case 'u':
      undbi = 1;
      break;
    default:
      fprintf(stderr, "tx: bad option '%c'\n", argc);
      return 1;
    }
  return 0;
}
void help(void) {}
