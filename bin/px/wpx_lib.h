#ifndef WPX_LIB_H_
#define WPX_LIB_H_

#include "px.h"
#include "pxdefs.h"

extern void do302(const char *loc);
extern void do400(const char *msg400);
extern void do404(void);
extern void wpx_print_hdr(void);
extern void wpx_print_hdr_text(void);
extern void wpx_print_hdr_xml(void);
extern void wpx_print_hdr_json(void);
extern void wpx_print_hdr_pdf(const char *pname);
extern void wpx_print_hdr_zip(const char *zname);

extern const char *wpx_last_error(void);

extern void wpx_cat_xml(const char *form, const char *file);

#endif/*WPX_LIB_H_*/
