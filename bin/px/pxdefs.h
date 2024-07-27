#ifndef PXDEFS_H_
#define PXDEFS_H_

#define PX_TMP_PAT    	"tmp/px/is.XXXXXX"
#define SRCHTMP 	"s.XXXXXX"
#define ADHOCTMP 	"a.XXXXXX"

#define PX_ERROR_START "px: error: "
#define QX_ERROR_START "qx: error: "
#define WX_ERROR_START "wx: error: "

#define px_err(...) px_mesg(PX_ERROR_START, __VA_ARGS__)
#define qx_err(...) px_mesg(QX_ERROR_START, __VA_ARGS__)
#define wx_err(...) px_mesg(WX_ERROR_START, __VA_ARGS__)

#define PX_STEP_0E	0x0000001
#define PX_STEP_1L	0x0000002
#define PX_STEP_2S	0x0000004
#define PX_STEP_3Z	0x0000008
#define PX_STEP_4P	0x0000010
#define PX_STEP_5W	0x0000020
#define PX_STEP_6C	0x0000040
#define PX_STEP_7F	0x0000080
#define PX_STEP_8O	0x0000100
#define PX_PARM_9A	0x0000200 /* AAPI */
#define PX_PARM_9U	0x0000400 /* UI MODE */

enum px_ret { PX_DONE=0, PX_ERROR=1, PX_CONTINUE=2};

enum px_err { PX_ER_NO_ERROR , PX_ER_BAD_PROJECT , PX_ER_BAD_LANG , PX_ER_BAD_TRAN };

extern int file_copy_lines(const char *from, const char *to, int start, int end);
extern const char *lang32(const char *l);
extern const char *px_mesg(const char *wpq, const char *fmt, ...);
extern unsigned char **px_loadfile_lines3(unsigned const char *fname, size_t *nlines, unsigned char **fmem);
extern unsigned char *px_loadfile_error(void);
extern unsigned char *px_loadfile(unsigned const char *fname, size_t *nbytes);

extern const char *s123[], *s12[], *s1[];
extern int precomputing;

#include <stdio.h>
struct isp;
extern void px_print_ip(FILE *fp, struct isp *ip);

#endif/*PXDEFS_H_*/
