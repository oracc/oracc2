#ifndef ROCO_H_
#define ROCO_H_

#include <hash.h>
#include <list.h>

/* Row-and-column type
 *
 * These routines support variable numbers of columns per row.
 *
 */
struct roco
{
  unsigned char *mem;    /* memory allocated for input */
  const char *file;      /* file name; - for stdin */
  unsigned char **lines; /* lines in a .tab/.tsv file, created by loadfile_lines3 */
  size_t nlines;	 /* number of lines */
  unsigned char ***rows; /* rows, an array of ptr to unsigned char ** */
  unsigned char **cols;  /* columns, i.e., array of unsigned char * to items after <TAB>;
			    empty string if <TAB><TAB> */
  int fields_from_row1;  /* take field names from row1 for xml output */
  const char *xmltag;    /* tag to wrap XML output */
  const char *rowtag;    /* tag to wrap row output */
  const char *celtag;    /* tag to wrap cell output */
  const char *class;     /* class for xmltag */
  const char *empty_row; /* row of maxcols tabs; for Roco in j_list, maxcols-1*'\t' */
  Hash *fields;		 /* Hash of field names to index in row */
  Hash *hdata;		 /* Hash of cell1 to row */
  int linkcells;	 /* The cells are Link* not char * */
  int maxcols;
  int joiner;
  int row1_literal;	/* Do not pass row1 through the roco_format template */
  int row1_fields_omit; /* Do not output row1 if it is fields */
  int skip_initial_lines;/* roco hash skips this many lines at start*/
  void *user;
};

typedef struct roco Roco;

typedef void (*Roco_row_hook)(Roco *r, int i, FILE *fp);

extern List *r_list;

#if 1
extern Roco_row_hook roco_row_hook_outer, roco_row_hook;
extern void roco_xml_row_hooks(Roco_row_hook outer, Roco_row_hook rh);
#else
extern Roco_row_hook roco_row_hook_outer, roco_row_hook_o, roco_row_hook_c;
extern void roco_xml_row_hooks(Roco_row_hook outer, Roco_row_hook o, Roco_row_hook c);
#endif

extern const char *roco_colorder;
extern const char *roco_format;
extern int roco_html_ns, roco_esp_ns, roco_ood_ns, roco_xmd_ns;
extern int roco_newline;
extern int roco_no_void_xml;
extern int roco_swap_axes;
extern int roco_xmlify;

/* Keep this on one line so GCC errors show the whole prototype
 */
extern Roco *roco_create(int rows, int cols);
extern void roco_field_indexes(Roco *r);
extern Hash *roco_hash(Roco *r);
extern void roco_hash_hash(Hash *h, Roco *r);
extern Roco *roco_load(const char *file, int fieldsr1, const char *xtag, const char *rtag, const char *ctag, const char *class);
extern void roco_row_format(FILE*fp, const unsigned char **row);
extern Roco *roco_swap(Roco *r);
extern void roco_write(FILE *fp, Roco *r);
extern void roco_write_trtd(FILE *fp, Roco *r);
extern void roco_write_xml(FILE *fp, Roco *r);

#define roco_load1(x) roco_load((x),0,NULL,NULL,NULL,NULL)

extern void roco_reorder(Roco *r, int left, int right);
extern void roco_fields_row(Roco *r, const char **f);
extern const char *roco_z_format(List *lp, Roco *r);
extern void roco_empty_row(Roco *r);
extern Hash *roco_hash_r(Roco *r);
extern char *roco_row_template(Roco *r, char **v, int add_empty);

#endif/*ROCO_H_*/
