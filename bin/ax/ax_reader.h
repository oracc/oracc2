#ifndef AX_READER_H_
#define AX_READER_H_

enum axr_source { axr_stdin , axr_files , axr_qids, axr_list , axr_qid_list };

#define AXR_BUFLEN 8192 /*16384*/

struct axrinfo 
{
  const char *proj;
  void *list;
  List *filenames_list;
  char input_buf[AXR_BUFLEN+1];
  size_t input_len;
  FILE *source;
  char *fname;
  const char **todo;
  int more_sources;
  int this_one_is_done;
  enum axr_source from;
  Pool *pool;
  int stop; /* set to 1 when runexpat_stop is used to exit XML
	       processing from within a handler */
};

extern struct axrinfo *rip;

extern void ax_read(const char *proj, enum axr_source from, void *list);

#endif/*AX_READER_H_*/
