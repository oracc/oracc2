#ifndef ISP_H_
#define ISP_H_

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

struct isp
{
  int curr_step;
  int steps[ISP_STEP_80];
  void *stepdata[ISP_STEP_80];
  const char *oracc;
  const char *project;
  const char *list_name;
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
  const char *list_path;
  const char *host_path;
  const char *err;
  Pool *p;
};

struct builtinstab
{
  const char *name;
  int step;
};

#endif/*ISP_H_*/
