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
  const char *glossary;
  const char *oxid;
  const char *pxid;
  P4arg *args;
  int nargs;
  const char *err;
  int status;
  int adhoc;
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
extern char const**p4url_vec(P4url *p);
extern struct urlkeytab *urlkeys (register const char *str, register size_t len);
extern struct qsoptionstab *qsoption (register const char *str, register size_t len);

#endif/*P4URL_H_*/
