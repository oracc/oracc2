#include <oraccsys.h>
#include <vid.h>

const char *curr_index, *curr_project;

int
main(int argc, char **argv)
{
  char buf[32], *s;
  options(argc, argv, "i:p:");

#if 0
  const char *vids = se_file(curr_project, curr_index, "vid.dat");
#endif
  
  struct vid_data *vp = vid_init();
  while ((s = fgets(buf, 32, stdin)))
    vid_new_id(vp, s);    
  vid_dump_data(vp, "vidx.dat");
  vid_term(vp);
}
void help(void) {}
int
opts(int argc, const char *str)
{
  switch (argc)
    {
    case 'i':
      curr_index = str;
      break;
    case 'p':
      curr_project = str;
      break;
    default:
      fprintf(stderr, "vidx: bad option %c\n", argc);
      exit(1);
      break;
    }
  return 0;
}
