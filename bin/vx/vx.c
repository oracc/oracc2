#include <oraccsys.h>
#include "vx.h"

int blank_lines = 0, s_words = 0;
Omode conllo_text = 0;
Omode output_mode = 0;
const char *translation_fn = NULL;

int
main(int argc, char *const *argv)
{
  gdl_flex_debug = gdldebug = 0;
  gdl_unicode = 1;
  setlocale(LC_ALL, ORACC_LOCALE);
  mesg_init();
  options(argc, argv, "bcCIOt:");

  if (!output_mode)
    output_mode = OM_IDENTITY;

  /* Pre-load setup */
  switch (output_mode)
    {
    case OM_CONLLO:
      vx_attr_p = vx_attr_xmlid;
      xmlid_h = hash_create(1024);
      break;
    default:
      ;
    }

  Tree *tp = vx_load(argv[optind]);

  switch (output_mode)
    {
    case OM_CATF:
      vx_catf(tp, stdout);
      break;
    case OM_IDENTITY:
      vx_identity(tp, stdout);
      break;
    case OM_CONLLO:
      vx_conllo(tp, stdout);
      break;
    default:
      fprintf(stderr, "vx: no output method for output_mode = %d\n", output_mode);
      break;
    }
  mesg_print(stderr);
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'b':
      blank_lines = 1;
      break;
    case 'c':
      conllo_text = OM_CATF;
      break;
    case 'C':
      output_mode = OM_CATF;
      break;
    case 'o':
      conllo_text = OM_OATF;
      break;
    case 'O':
      output_mode = OM_CONLLO;
      break;
    case 'I':
      output_mode = OM_IDENTITY;
      break;
    case 't':
      translation_fn = arg;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void
help(void)
{
  fprintf(stderr, "vx -[MODE] [FILE]\n");
}
