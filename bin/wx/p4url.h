#ifndef P4URL_H_
#define P4URL_H_

typedef struct p4arg
{
  const char *option;
  const char *value;
} P4arg;

typedef struct p4url
{
  char *u;	/* copy of URL to chop up */
  char *q;	/* copy of QUERY_STRING to chop up */
  const char *project;
  const char *file;
  const char *glossary;
  const char *oxid;
  const char *pxid;
  const char *frag;
  P4arg *args;
  int nargs;
  const char *err;
  int status;
  int adhoc;
  int ood;
  const char *exe; /* p = px; q = qx */
} P4url;

struct qsoptionstab
{
  const char *name;
  char *value;
};

struct urlkeytab
{
  const char *name;
  char *option;
};

extern const char *docroot;

extern int p4url_eval(const char *u, const char *q);
extern int p4url(P4url *p, const char *u, const char *q);
extern void p4oid(P4url *p);
extern char const**p4url_vec(P4url *p);
extern struct urlkeytab *urlkeys (register const char *str, register size_t len);
extern struct qsoptionstab *qsoption (register const char *str, register size_t len);
extern int _is_ncname(const char *name);
extern const char *p4url_validate(const char *name, const char *value, const char *allow);
extern const unsigned char *p4url_guard(const char *name, const unsigned char *v);

#endif/*P4URL_H_*/
