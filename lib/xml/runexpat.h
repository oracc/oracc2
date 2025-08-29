#ifndef _RUNEXPAT_H
#define _RUNEXPAT_H
#include "expat.h"
enum isource { i_stdin , i_names , i_list };

extern const char *pi_file;
extern int pi_line;

#define  runexpat(from,list,sH,eH) runexpatNSuD(from,list,sH,eH,NULL,NULL)
#define  runexpatNS(from,list,sH,eH,ns) runexpatNSuD(from,list,sH,eH,ns,NULL)
#define  runexpatuD(from,list,sH,eH,uD) runexpatNSuD(from,list,sH,eH,NULL,uD)

extern const char *findAttr(const char **atts, const char *name);
extern void charData_discard(void);
extern XML_Char*charData_retrieve(void);

extern void runexpatNSuD(enum isource from, void *list,
			 XML_StartElementHandler startElement, 
			 XML_EndElementHandler endElement,
			 XML_Char *ns_sep_p, void *userData);

extern const char *get_xml_id(const char **atts);
extern const char *get_xml_lang(const char **atts);
extern void runexpat_omit_rp_wrap(void);

typedef const char ** (*xf_action_attr)(const char *xmlid, const char *name, const char **attr, FILE *fp);
typedef void (*xf_action_func)(const char *xmlid, const char *name, const char **attr, FILE *fp);
extern void xmlfilter(const char *fname, FILE *outfp);
extern void xmlfilter_actions(xf_action_attr attr, xf_action_func firstChild, xf_action_func lastChild);

extern XML_CommentHandler runexpatCommentHandler;

#endif /*_RUNEXPAT_H*/
