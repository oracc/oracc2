#ifndef ISP_H_
#define ISP_H_

#include <setjmp.h>

#include <memo.h>
#include <pool.h>
#include <dbi.h>

struct isp_list_loc
{
  const char *type;   /* list type: glo xis tis isp www */
  const char *lang;   /* lang prefix when type = xis */
  const char *method; /* how to find the list: tis xis file */
  const char *dbpath; /* dir of dbx for some tis and xis */
  const char *dbname; /* name of dbx for some tis and xis */
  const char *path;   /* path to file */
};

struct isp_srchdata
{
  const char *tmp;	/* path to search's tmpdir */
  const char *bar;  	/* path to sub/search.bar = entire search bar text */
};

struct isp_cache
{
  const char *sys;	/* the system cache directory: /home/oracc/tmp/isp
			   for search results; /home/oracc/www/is.d
			   for precompiled instance sets */
  const char *sub;	/* the project's cache directory */
  const char *list;	/* the list file */
  const char *sort; 	/* the list sorted by the requested permutation */
  const char *mol; 	/* the master outline for the list in cache.sort */
  const char *pgin;	/* the input for a page extracted from the cache.sort */
  const char *page;	/* the filename of the page-content for a zoomed slice; z=0 for no zoom */
  const char *zout;	/* the filename of the zoom-menu for a zoomed slice; z=0 for no zoom */
  const char *item;	/* the cache-dir for item data */
  const char *meta;	/* the filename of the metadata for the displayed item */
  const char *html;	/* the filename of the html content of the displayed item */
  const char *ltab;	/* the filename of the letters.tab if cache is a glossary */
  const char *hilite;	/* table of text-id\tword-ids to set the hilite on instance sets */
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

struct isp_itemdata
{
  const char **langs;
  const char *lmem;
  const char *item;
  const char *html;
  const char *dots;
  const char *index;
  const char *page;
  const char *zoom;
  const char *zpag;
  const char *prev;
  const char *next;
  const char *tmax;
  const char *xmdxsl;
  const char *bld;
  Dbi_index *dp;
};

struct isp_glosdata
{
  const char *dir;  	/* path to 02pub/cbd/LANG */
  const char *web;  	/* path to 02www/cbd/LANG */
  const char *let; 	/* letter to zoom to */
  const char *ent; 	/* entry item to display */
  const char *xis; 	/* instance-set--not necessarily XML IS--from -l/list= */
  const char *ltab; 	/* pub/cbd/LANG/letter-ids.tab */
  const char *lbase;	/* entry_ids or an LXXX id from letter_ids.tab */
  const char *lpath;	/* list-path dir+lbase */
  const char *ecpath; 	/* path to cbd/[LANG]entry-counts.tab */
  const char *emax; 	/* entry count from entry-counts.tab */
  const char *ipath;	/* item-path: www/PROJECT/cbd/LANG/ITEM.html */
};

#include "../pxdefs.h"

typedef struct isp
{
  int curr_step;
  int steps[PX_STEP_8O];
  void *stepdata[PX_STEP_8O];
  const char *oracc;	/* the system oracc install, usually /home/oracc */
  const char *from; /* list or srch */
  const char *data; /* dcat dtxt dtra dglo */
  const char *show; /* rcat rtxt rtra rglo itxt iart */
  struct isp_cache cache;
  const char *project;
  const char *projdir;
  const char *list_name;
  struct isp_list_loc lloc;
  struct isp_sort sort;
  struct ispo is;
  struct isp_mapdata md1;
  struct isp_mapdata md2;
  struct isp_itemdata itemdata;
  struct isp_glosdata glosdata;
  struct isp_srchdata srchdata;
  struct outline *op;
  int op_nlevels;
  const char *dors; /* 0 = default ; 1 = special */
  const char *perm; /* permutation expressed as 123 132, etc. (see isp_sort.c) */
  const char *zoom;
  const char *page;
  const char *psiz;
  const char *srch; /* search string from search.bar */
  const char *glos; /* glossary from URL */
  const char *item; /* a PQX for item display */
  const char *item_replace; /* a PQX replacement to use for proxy display */
  const char *bkmk; /* a note of the last item visited resuming page mode */
  const char *ceid; /* context engine id handling--xmd/xtf/cbd */
  const char *cemd; /* context engine mode */
  const char *lang; /* item translation lang */
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
  int debug;
  int verbose;
  int web;
  int argc;
  const char *referer;
  const char *tmpdir;
  const char **argv;
  struct xpd *xpd;
  Pool *p;
  Memo *tmem;
  jmp_buf errjmp;
} Isp;

#if 0
/* p3-pager.plx will not be used any more */
  int p3;	    /* produce outline and results and then output name of
		       temp dir for p3-pager.plx to build output
		       page */
#endif

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
