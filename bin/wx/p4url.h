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
  const char *id;
  P4arg *args;
  const char *err;
  int status;
} P4url;

extern const char *docroot;

extern int p4url(P4url *p, const char *u, const char *q);

#endif/*P4URL_H_*/
