#ifndef P4URL_H_
#define P4URL_H_

Typedef struct p4arg
{
  const char *option;
  const char *value;
} P4arg;

Typedef struct p4url
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

#endif/*P4URL_H_*/
