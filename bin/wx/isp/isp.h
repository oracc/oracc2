#ifndef ISP_H_
#define ISP_H_

#include <memo.h>
#include <pool.h>

#define ISP_ERROR_START "isp: error: "
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
  const char *mol; 	/* the master outline for the list in cache.sort */
  const char *pgin;	/* the input for a page extracted from the cache.sort */
  const char *page;	/* the page in a zoomed slice; z=0 for no zoom */
  const char *zout;	/* the outline for a zoomed slice; z=0 for no zoom */
};

/* Pointers to project config entries */
struct isp_sort
{
  const char *default_sort;
  const char *default_select;
  const char *special_sort;
  const char *special_select;  
};

struct item
{
  unsigned char   *s;
  unsigned char   *pq;
  unsigned char   *qpq; /* qualified pq */
  Unsigned32   skey;
  Signed32 lkey;
  Signed32 grp;
};

struct outline
{
  struct si_cache *sic;
  char *hdr;
  Unsigned32 *poffsets;
  Unsigned32 *icounts;
  Signed32 page;
  Signed32 count;
};

/* Top-level structure for the outline */
struct ispo
{
  unsigned char **zlines;
  size_t zmax;
  int zlev;
  List *l; /* data items are isph w level = 1 */
};

/* Per-heading structure for a zoom line */
struct isph
{
  int zoom;
  int level;
  unsigned char *h;
  int zimx; /* z-item max */
  List *l;
};

/* Zoom line split up into headers and items */
struct ispz
{
  unsigned char *h1;
  unsigned char *h2;
  unsigned char *h3;
  unsigned char *items;
  int count;
};

struct page
{
  Signed32 used;
  char **p;
};

struct isp_mapdata
{
  int zmax; /* max value for zoom */
  int zimx; /* max value for zoom's item */
  unsigned long htell;
  int hlen;
  unsigned long ptell;
  int plen;
};

typedef struct isp
{
  int curr_step;
  int steps[ISP_STEP_8O];
  void *stepdata[ISP_STEP_8O];
  const char *oracc;	/* the system oracc install, usually /home/oracc */
  struct isp_cache cache;
  const char *project;
  const char *list_name;
  struct isp_list_loc lloc;
  struct isp_sort sort;
  struct ispo is;
  struct isp_mapdata md1;
  struct isp_mapdata md2;
  struct outline *op;
  int op_nlevels;
  const char *dors; /* 0 = default ; 1 = special */
  const char *perm; /* permutation expressed as 123 132, etc. (see isp_sort.c) */
  const char *zoom;
  const char *page;
  const char *psiz;
  const char *ceid; /* context engine id handling--xmd/xtf/cbd */
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
  const char *errx;
  int force;
  int olev;	    /* outline levels, i.e., number of headings
		       without any appended designation */
  int p3;	    /* produce outline and results and then output name of
		       temp dir for p3-pager.plx to build output
		       page */
  int verbose;
  int web;
  Pool *p;
  Memo *tmem;
} Isp;

struct idactions
{
  const char *name;
  void (*func)(Isp *ip, struct idactions *idap, FILE *fp);
};

struct ispargstab
{
  const char *name;
  int step;
};

#include "iss_redblack.h"
#include "iss_sortinfo.h"

#include "all.proto"

extern int sic_size;
extern int *sortfields;
extern struct si_cache **sicache;
extern struct sortinfo *sip;

extern int nheadfields;
extern int *headfields;
extern int nsortfields;

#endif/*ISP_H_*/
