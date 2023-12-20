#ifndef JSON_H_
#define JSON_H_

#include <stdlib.h>
#include <stdio.h>

enum jstate { jstart , jarray , jobject , jmember , jelement , jstring , jvalue };

extern const unsigned char *jsonify(const unsigned char *j);

extern enum jstate jw_state(void);
extern void jw_init(FILE *fp);
extern void jw_term(void);
extern void jw_array_o(void);
extern void jw_array_c(void);
extern void jw_object_o(void);
extern void jw_object_c(void);
extern void jw_member(const char *name);
extern void jw_string(const char *s);
extern void jw_number(int i);
extern void jw_true(void);
extern void jw_false(void);
extern void jw_null(void);
extern void jw_strmem(const char *m, const char *v);

#endif/*JSON_H_*/
