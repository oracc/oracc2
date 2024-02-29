#include <oraccsys.h>
#include "isp.h"

static struct isp *opt_ip;

int
isp_options(int argc, char **argv, struct isp *ip)
{
  int ret;
  opt_ip = ip;
  ret = options(argc, argv, "ELSZPWCFOj:l:z:p:s:k:h:m:u:c:l:a:");
  opt_ip = NULL;
  if (ret)
    ip->err = "processing options";
  return ret;
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'E':
      opt_ip->steps[ISP_STEP_0E] = 1;
      break;
    case 'L':
      opt_ip->steps[ISP_STEP_1L] = 1;
      break;
    case 'S':
      opt_ip->steps[ISP_STEP_2S] = 1;
      break;
    case 'Z':
      opt_ip->steps[ISP_STEP_3Z] = 1;
      break;
    case 'P':
      opt_ip->steps[ISP_STEP_4P] = 1;
      break;
    case 'W':
      opt_ip->steps[ISP_STEP_5W] = 1;
      break;
    case 'C':
      opt_ip->steps[ISP_STEP_6C] = 1;
      break;
    case 'F':
      opt_ip->steps[ISP_STEP_7F] = 1;
      break;
    case 'O':
      opt_ip->steps[ISP_STEP_8O] = 1;
      break;
    case 'w':
      opt_ip->web = 1;
      break;
    case 'j':
      opt_ip->project = arg;
      break;
    case 'l':
      opt_ip->list_name = arg;
      break;
    case 'z':
      opt_ip->zoom = arg;
      break;
    case 'p':
      opt_ip->page = arg;
      break;
    case 's':
      opt_ip->size = arg;
      break;
    case 'c':
      opt_ip->cemd = arg;
      break;
    case 'n':
      opt_ip->lang = arg;
      break;
    case 'm':
      opt_ip->xhmd = arg;
      break;
    case 'k':
      opt_ip->pack = arg;
      break;
    case 'h':
      opt_ip->host = arg;
      break;
    case 'a':
      opt_ip->aapi = arg;
      break;
    case 'u':
      opt_ip->uimd = arg;
      break;
    case 'v':
      ++opt_ip->verbose;
      break;
    default:
      return 1;
    }
  return 0;
}
