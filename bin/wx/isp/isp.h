#ifndef ISP_H_
#define ISP_H_

#include <pool.h>

#define ISP_TMP_PAT    	"tmp/isp/is.XXXXXX"

#define ISP_STEP_0E	0x0000001
#define ISP_STEP_1L	0x0000002
#define ISP_STEP_2S	0x0000004
#define ISP_STEP_3Z	0x0000008
#define ISP_STEP_4P	0x0000010
#define ISP_STEP_5W	0x0000020
#define ISP_STEP_6C	0x0000040
#define ISP_STEP_7F	0x0000080
#define ISP_STEP_8O	0x0000100
#define ISP_PARM_9A	0x0000200 /* AAPI */
#define ISP_PARM_9U	0x0000400 /* UI MODE */

struct isp_list_loc
{
  const char *type;   /* list type: xis tis isp www */
  const char *lang;   /* lang prefix when type = xis */
  const char *method; /* how to find the list: tis xis file */
  const char *dbpath; /* dir of dbx for some tis and xis */
  const char *dbname; /* name of dbx for some tis and xis */
  const char *path;   /* path to file */
};

struct isp_cache
{
  const char *sys;	/* the system cache directory: /home/oracc/tmp/isp
			   for search results; /home/oracc/www/is.d
			   for precompiled instance sets */
  const char *sub;	/* the list's cache directory */
  const char *list;	/* the list file */
  const char *sort; 	/* the list sorted by the requested permutation */
  const char *page;	/* the page in a zoomed slice; z=0 for no zoom */
};

/* Pointers to project config entries */
struct isp_sort
{
  const char *default_sort;
  const char *default_select;
  const char *special_sort;
  const char *special_select;  
};

typedef struct isp
{
  int web;
  int curr_step;
  int steps[ISP_STEP_8O];
  void *stepdata[ISP_STEP_8O];
  const char *oracc;	/* the system oracc install, usually /home/oracc */
  struct isp_cache cache;
  const char *project;
  const char *list_name;
  struct isp_list_loc lloc;
  struct isp_sort sort;
  const char *mode; /* 0 = default ; 1 = special */
  const char *perm; /* permutation expressed as 123 132, etc. (see isp_sort.c) */
  const char *zoom;
  const char *page;
  const char *size;
  const char *cemd; /* context engine mode */
  const char *lang; /* context engine lang */
  const char *xhmd; /* xml or html mode */
  const char *uimd; /* uimode */
  const char *pack; /* packaging type */
  const char *host; /* host page for pack=esp2 */
  const char *aapi;
  const char *host_path;
  const char *tmp_dir;
  const char *err;  
  int verbose;
  Pool *p;
} Isp;

struct ispargstab
{
  const char *name;
  int step;
};

#include "iss_redblack.h"
#include "iss_sortinfo.h"

#include "all.proto"

#endif/*ISP_H_*/
