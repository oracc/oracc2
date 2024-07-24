#ifndef WPX_LIB_H_
#define WPX_LIB_H_

#include "px.h"
#include "pxdefs.h"

extern void do302(const char *loc);
extern void do400(const char *msg400);
extern void do404(void);
extern void wpx_print_hdr(Isp*ip);
extern void wpx_print_hdr_text(Isp*ip);
extern void wpx_print_hdr_xml(Isp*ip);
extern void wpx_print_hdr_json(Isp*ip);
extern void wpx_print_hdr_pdf(Isp*ip, const char *pname);
extern void wpx_print_hdr_zip(Isp*ip, const char *zname);

extern const char *wpx_last_error(void);

extern void wpx_cat_html(Isp*ip, const char *form, const char *file);
extern void wpx_cat_xml(Isp*ip, const char *form, const char *file);

#endif/*WPX_LIB_H_*/
