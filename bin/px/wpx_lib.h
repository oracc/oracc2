#ifndef WPX_LIB_H_
#define WPX_LIB_H_

#include "px.h"
#include "pxdefs.h"

extern const char *cgi_arg(const char *arg, const char *val);
extern void do302(const char *loc);
extern void do400(const char *msg400);
extern void do404(void);
extern char *wpx_find_www_file(const char *project, const char *dir, const char *basename, const char *ext);
extern char *wpx_find_art_file(const char *project, const char *glos, const char *oid, const char *form);
extern char *wpx_find_glo_file(const char *project, const char *glos, const char *form);
extern char *wpx_find_pqx_file(const char *project, const char *pqid, const char *ext);
extern char *wpx_find_pqx_xtr(const char *project, const char *pqid, const char *code, const char *lang);
extern void wpx_print_hdr(void);
extern void wpx_print_hdr_text(void);
extern void wpx_print_hdr_xml(void);
extern void wpx_print_hdr_json(void);
extern void wpx_print_hdr_pdf(const char *pname);
extern void wpx_print_hdr_zip(const char *zname);

extern const char *wpx_last_error(void);

extern void wpx_cat_file(const char *form, const char *file);

#endif/*WPX_LIB_H_*/
