#include <oraccsys.h>
#include <tsv.h>

int verbose = 0;
const char *prog = "tx";
int major_version = 1;
int minor_version = 0;
const char *usage_string = " [-t tsv] [-d dir] [-n name] [-k key]";
  
const char *arg_dir, *arg_key, *arg_name, *arg_tsv;

int space_newlines, undbi;

int
main(int argc, char **argv)
{
  program_values(prog, major_version, minor_version, usage_string, NULL);
  options(argc, argv, "d:k:n:st:u");

  if (arg_key)
    {
      /* look up key and dump results */
      int res = tsv_one_off(arg_tsv, arg_dir, arg_name, arg_key);
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
  else
    usage();

  return 0;
}

/* More options:
 *
 * -o output_fn
 * -i print key as first column
 * -f first line is fields
 */

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'd':
      arg_dir = arg;
      break;
    case 'k':
      arg_key = arg;
      break;
    case 'n':
      arg_name = arg;
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
void help() {}
