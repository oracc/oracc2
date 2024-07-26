#ifndef ISP_H_
#define ISP_H_

#include <setjmp.h>

#include <memo.h>
#include <pool.h>
#include <dbi.h>

struct isp_list_loc
{
  const char *type;   /* list type: glo xis tis isp www xtl */
  const char *lang;   /* lang prefix when type = xis */
  const char *method; /* how to find the list: tis xis file xtl */
  const char *dbpath; /* dir of dbx for some tis and xis */
  const char *dbname; /* name of dbx for some tis and xis */
  const char *path;   /* path to file; this should be a list--it is
			 sometimes a source list which is copied to
			 the cache and sometimes a list generated into
			 the cache (including search outputs and xtl
			 lists) */
};

struct isp_srchdata
{
  const char *tmp;	/* path to search's tmpdir */
  const char *bar;  	/* path to tmp/search.bar = entire search bar text */
  long int count;	/* count of results read from tmp/count */
  const char *gran;	/* granularity as t(ext) r(ecord) f(ield) w(word) g(rapheme) */
  const char *list;	/* the final tmpdir element which is used for data-list */
  int new;		/* 0 when reusing a search list, 1 when the srch was just done */
  int adhoc;
};

struct isp_cache
{
  const char *sys;	/* the system cache directory, default /home/oracc/www/p4.d */
  const char *project;	/* the project's cache directory */
  const char *sub;	/* the current cache with list used for inputs; may include 02pub/p4.d */
  const char *out;	/* the current cache with list used for outputs; does not include 02pub/p4.d */
  const char *list;	/* the list file */
  const char *sort; 	/* the list sorted by the requested permutation;
			   may be in PROJECT/p4.d/LIST or SYSTEM/p4.d/PROJECT/LIST */
  const char *csi; 	/* the compiled sort info (.csi) file */
  const char *tsv; 	/* the computed pages tsv list in cache.sort */
  const char *max; 	/* maximum zoom items tsv list in cache.sort */
  const char *mol; 	/* the master outline for the list in cache.sort */
  const char *pkey;	/* the page key, e.g., z0p3 */
  const char *pgin;	/* the page input, only used for glos */
  const char *page;	/* the filename of the page-content for a zoomed slice; z=0 for no zoom */
  const char *zout;	/* the filename of the zoom-menu for a zoomed slice; z=0 for no zoom */
  const char *item;	/* the cache-dir for item data */
  const char *prox;	/* the cache-dir for proxied item data */
  const char *meta;	/* the filename of the metadata for the displayed item */
  const char *html;	/* the filename of the html content of the displayed item */
  const char *ltab;	/* the filename of the letters.tab if cache is a glossary */
  const char *hilite;	/* table of text-id\tword-ids to set the hilite on instance sets */
  const char *pub; 	/* set to "02pub/p4.d" to create project with p4.d in project/02pub;
			   gets reset to full p4d path */
  const char *use;	/* convenience pointer to either cache.project
			   or cache.pub depending on whether page tsv
			   is precomputed or not */
  const char *txtindex; /* directory for txtindex.lst if item key was found there */
  const char *t_sort; 	/* the dir ending in /t that the text data list lives in */
  const char *t_tsv; 	/* the computed pages tsv list in cache.sort/t */
  const char *t_max; 	/* maximum zoom items tsv list in cache.sort/t */
  const char *t_mol; 	/* master outline for text list in cache.sort/t */
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
  const char *langs;	/* all the langs this item is available in */
  const char **langp;	/* langs as array */
  size_t nlangs;
  const char *xtflang;	/* the lang to use for the next xtf item call */ 
  const char *lmem;
  const char *item;
  const char *block;
  const char *proj; /* the item's project as opposed to the host project, possibly a proxy */
  const char *htmd; /* the path to and including /htm */
  const char *html;
  const char *dots;
  const char *index;
  const char *page;
  const char *pindex;
  const char *zoom;
  const char *zpag;
  const char *zindex;
  const char *prev;
  const char *next;
  const char *tmax;
  const char *xmdxsl;
  const char *bld;
  Dbi_index *dp;
  Dbi_index *hilitedb;
  int not; /* 2 = no xmd (i.e., text not in project); 3 = no xtf */
};

struct isp_glosdata
{
  const char *dir;  	/* path to 02pub/cbd/LANG */
  const char *web;  	/* path to 02www/cbd/LANG */
  const char *let; 	/* letter to zoom to */
  const char *lmax; 	/* max entries in curr 'let' */
  const char *ent; 	/* entry item to display */
  const char *xis; 	/* instance-set--not necessarily XML IS--from -l/list= */
  const char *ltab; 	/* pub/cbd/LANG/letter-ids.tab */
  const char *lbase;	/* entry_ids or an LXXX id from letter_ids.tab */
  const char *lpath;	/* list-path dir+lbase */
  const char *ecpath; 	/* path to cbd/[LANG]entry-counts.tab */
  const char *emax; 	/* entry count from entry-counts.tab */
  const char *ipath;	/* item-path: www/PROJECT/cbd/LANG/ITEM.html */
};

struct isp_config
{
  int leftmenu;
  int select;
  const char *sort_fields;
  const char *sort_labels;
  const char *head_template;
  const char *cat_fields;
  const char *cat_links;
  const char *cat_widths;
};

#include "../pxdefs.h"

struct isoz;

typedef struct isp
{
  int curr_step;
  int steps[PX_STEP_8O];
  void *stepdata[PX_STEP_8O];
  const char *oracc;	/* the system oracc install, usually /home/oracc */
  const char *from; /* list or srch */
  const char *data; /* dcat dtxt dtra dglo */
  const char *show; /* rcat rtxt rtra rglo itxt iart */
  const char *project;
  const char *projdir;
  const char *list_name;
  int op_nlevels;
  const char *dors; /* 0 = default ; 1 = special */
  const char *perm; /* permutation expressed as 123 132, etc. (see isp_sort.c) */
  const char *zoom;
  const char *page;
  const char *psiz;
  const char *srch; /* search string from search.bar */
  const char *glos; /* glossary from URL */
  const char *item; /* a PQX for item display */
  const char *hili; /* a PQX line/word/grapheme ID to hilite */
  const char *item_replace; /* a PQX replacement to use for proxy display */
  const char *bkmk; /* a note of the last item visited resuming page mode */
  const char *ceid; /* context engine id handling--xmd/xtf/cbd */
  const char *cemd; /* context engine mode */
  const char *lang; /* item translation lang */
  const char **halp;/* http_accept_language langs as array */
  const char *xhmd; /* xml or html mode */
  const char *part; /* page=pager; full=meta+tlit+xlat; plus=tlit+xlat */
  const char *form; /* also xml or html mode, in URL/QUERY_STRING; are
		     * both needed?  with form if it is NULL we
		     * provide pager output, otherwise the html/xml
		     * file directly; could also use this for json,
		     * pdf, tei so maybe retiring xhmd is best.
		     */
  const char *what; /* text score block sources */
  const char *wrap; /* the wrapper program for making form from text, as in progtab.g name field */
  const char *uimd; /* uimode */
  const char *pack; /* packaging type */
  const char *host; /* host page for pack=esp2 */
  const char *aapi;
  const char *host_path;
  const char *sig;
  const char *tmp_dir;
  const char *err;
  const char *errx;
  const char *pui;
  int nowhat;
  int force;
  int olev;	    /* outline levels, i.e., number of headings
		       without any appended designation */
  int debug;
  int noheader;
  int pub_output;
  int verbose;
  int web;
  int zlev; 	/* not sure this is necessary with refactor */
  int argc;
  enum px_err pxerr;
  const char *referer;
  const char *tmpdir;
  const char **argv;
  struct xpd *xpd;
  Pool *p;
  Memo *tmem;
  Memo *isozmem;
  jmp_buf errjmp;
  struct isp_cache cache;
  struct isp_config *curr_cfg;
  struct isp_config default_cfg;
  struct isp_config special_cfg;
  struct isp_glosdata glosdata;
  struct isp_itemdata itemdata;
  struct isp_list_loc lloc;
#if 0
  struct isp_mapdata md1;
  struct isp_mapdata md2;
#endif
  struct isp_srchdata srchdata;
  struct isoz *iop;
  int zmax;
} Isp;

/*
  struct ispo is;
 */
struct ispargstab
{
  const char *name;
  int step;
};

extern int iso_master(Isp *ip, const char *mol);
extern int iso_zoutline(Isp *ip);
extern int iss_sort(Isp *ip);

#include "all.proto"

#endif/*ISP_H_*/
