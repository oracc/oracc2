#ifndef AX_READER_H_
#define AX_READER_H_

enum axr_source { axr_stdin , axr_files , axr_qids, axr_list , axr_qid_list };

#define AXR_BUFLEN 8*1024; /*16384*/

struct axrinfo 
{
  const char *proj;
  void *list;
  List *filenames_list;
  char *input_buf;
  size_t input_buf_alloced;
  size_t input_len;
  FILE *source;
  char *fname;
  const char **todo;
  int more_sources;
  int this_one_is_done;
  enum axr_source from;
  Pool *pool;

  char *amp_ptr; /* pointer to current &-line in input_buf */
  char *end_ptr; /* pointer to next &-line in input_buf */
  
  int stop; /* set to 1 when atf_stop is used to exit atf */
};

extern struct axrinfo *rip;

extern void ax_read(const char *proj, enum axr_source from, void *list);

#endif/*AX_READER_H_*/
